/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "peripherals.h"
#include "time.h"

#include "../uvita/algorithm/uvita_algo.h"
#include "../uvita/camera/uvita_camera.h"
#include "../uvita/display/uvita_display.h"
#include "../uvita/camera/uvita_pxp.h"
#include "../uvita/message/uvita_queue.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define UVITA_AlgoTaskPriority (configMAX_PRIORITIES - 3)
#define UVITA_CameraTaskPriority (configMAX_PRIORITIES - 2)
#define UVITA_DisplayTaskPriority (configMAX_PRIORITIES - 1)

#define MAX_LOG_LENGTH 4
#define MAX_QUEUE_LENGTH 10
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void UVITA_DEMO_AlgoTask(void* pvParameters);
static void UVITA_DEMO_CameraTask(void* pvParameters);
static void UVITA_DEMO_DisplayTask(void* pvParameters);
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_BootClockRUN();
    BOARD_ResetDisplayMix();
    BOARD_EarlyInitCamera();
    BOARD_InitLpuartPins();
    BOARD_InitMipiPanelPins();
    BOARD_InitMipiCameraPins();
    BOARD_InitCodecPins();
    BOARD_InitDebugConsole();
    Time_Init(1);

    UVITA_DEMO_InitPxp();
    UVITA_MessageInit(MAX_QUEUE_LENGTH, MAX_LOG_LENGTH);
    uvita_bmtest();

    if (xTaskCreate(UVITA_DEMO_CameraTask, "camera", 512, NULL, UVITA_CameraTaskPriority, NULL) !=
        pdPASS) {
        PRINTF("Camera Task creation failed!.\r\n");

        while (1)
            ;
    }

#if UVITA_DISPLAY_ENABLE

    if (xTaskCreate(UVITA_DEMO_DisplayTask, "display", 512, NULL, UVITA_DisplayTaskPriority, NULL) !=
        pdPASS) {
        PRINTF("Display Task creation failed!.\r\n");

        while (1)
            ;
    }

#endif

    if (xTaskCreate(UVITA_DEMO_AlgoTask, "algorithm", 1024, NULL, UVITA_AlgoTaskPriority, NULL) !=
        pdPASS) {
        PRINTF("Algorithm task creation failed!.\r\n");

        while (1)
            ;
    }

    vTaskStartScheduler();

    for (;;)
        ;
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void UVITA_DEMO_AlgoTask(void* pvParameters)
{
    UVITA_DEMO_AlgoInit();
    UVITA_DEMO_AlgoRun();
}

static void UVITA_DEMO_CameraTask(void* pvParameters)
{
    UVITA_DEMO_InitCamera();
    UVITA_DEMO_CameraRun();
}

static void UVITA_DEMO_DisplayTask(void* pvParameters)
{
	UVITA_DEMO_InitDisplay();
    UVITA_DEMO_DisplayRun();
}


