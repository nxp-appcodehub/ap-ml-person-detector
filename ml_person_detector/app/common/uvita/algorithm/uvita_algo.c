/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <FreeRTOS.h>
#include "time.h"

#include "uvita_camera.h"
#include "uvita_algo.h"
#include "uvita_present.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
//uint8_t s_Buffer[UVITA_FULL_FRAME_H * UVITA_FULL_FRAME_W * 3];
//uvita_image_frame Frame = {UVITA_FULL_FRAME_H, UVITA_FULL_FRAME_W, UVITA_IMG_INT8_BGR888, (uint8_t*)s_Buffer};
uint8_t s_detBuffer[UVITA_DET_FRAME_H * UVITA_DET_FRAME_W * 3];
uvita_image_frame detFrame = {UVITA_DET_FRAME_H, UVITA_DET_FRAME_W, UVITA_IMG_INT8_BGR888, (uint8_t*)s_detBuffer};

static int ALGO_Finished = 1;

uvita_person_out pout;
uvita_person_out showout;

/* The PIN status */
volatile bool g_pinSet = false;

void UVITA_DEMO_Print(char* str)
{
    PRINTF("%s", str);
}

void UVITA_DEMO_AlgoInit(void)
{

}

void UVITA_DEMO_FrameAddr(uint8_t** detBuffer)
{
    if (ALGO_Finished == 0) {
        *detBuffer = NULL;
    } else {
        *detBuffer = s_detBuffer;
    }
}

void UVITA_DEMO_AlgoGUI(uvita_image_frame* frame)
{
    uvita_status ret;
    int lineWidth = 2;
	if (frame->height >= 480 && frame->width >= 640) {
		lineWidth = 4;
	}
    /* remap the boudning boxes according to detection frame and diaplay frame */
	showout.has_person = pout.has_person;
	for (int i = 0; i < pout.has_person; i++) {
		/* person boxes re-map to the original frame */
		showout.boxes[i].rect[RECT_LEFT_X] = (int)(frame->width * (float)pout.boxes[i].rect[RECT_LEFT_X] / detFrame.width + 0.5f);
		showout.boxes[i].rect[RECT_TOP_Y] = (int)(frame->height * (float)pout.boxes[i].rect[RECT_TOP_Y] / detFrame.height + 0.5f);
		showout.boxes[i].rect[RECT_RIGHT_X] = (int)(frame->width * (float)pout.boxes[i].rect[RECT_RIGHT_X] / detFrame.width + 0.5f);
		showout.boxes[i].rect[RECT_BOTTOM_Y] = (int)(frame->height * (float)pout.boxes[i].rect[RECT_BOTTOM_Y] / detFrame.height + 0.5f);
	}

    uvita_present_nxp_logo(frame);
    uvita_present_person(frame, showout, lineWidth, UVITA_PRESENT_YELLOW);

    if (ret != UVITA_SUCCESS) {
        PRINTF("uvita present error: %d\r\n", ret);
    }
}

void UVITA_DEMO_AlgoRun(void)
{
    uint32_t start_time, stop_time;
    uint32_t duration_ms = 0;
    int Algo_Recieve = 0;

    while (1) {
        if (UVITA_MessageRecieve(&Algo_Recieve, CAMERA_ALGO_QUEUE) != pdTRUE) {
            PRINTF("fail to receive new frame from camera to algorithm\r\n");
        }

        ALGO_Finished = 0;
        start_time = Time_Current();

        /* multiple person detection */
        uvita_dperson_run(&detFrame, &pout);

        stop_time = Time_Current();
        duration_ms = (stop_time - start_time) / 1000;

        ALGO_Finished = 1;

        PRINTF("[multi-person-detection]: counting %d, latency %d ms\r\n", pout.has_person, duration_ms);
    }
}

/**
 * @brief Bare metal speed ​​test for multiple person detection
 */
void uvita_bmtest(void)
{
	uint32_t start_time, stop_time;
	uint32_t duration_ms = 0;

	start_time = Time_Current();

	/* multiple person detection */
	uvita_dperson_run(&detFrame, &pout);

	stop_time = Time_Current();
	duration_ms = (stop_time - start_time) / 1000;

	PRINTF("[Bare-metal-multi-person-detection]: latency %d ms\r\n", duration_ms);

}

