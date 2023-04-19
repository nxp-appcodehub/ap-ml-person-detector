/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*System includes.*/
#include <stdio.h>
#include "uvita_queue.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Globals
 ******************************************************************************/
/* queue handle */
static QueueHandle_t camera_recieve_queue = NULL;
static QueueHandle_t camera_dispaly_queue = NULL;
static QueueHandle_t camera_algo_queue = NULL;
static QueueHandle_t algo_display_queue = NULL;
static QueueHandle_t algo_codec_queue = NULL;
static QueueHandle_t camera_pxp_queue = NULL;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*******************************************************************************
 * Logger functions
 ******************************************************************************/
/*!
 * @brief log_add function
 */
BaseType_t UVITA_MessageAdd(int* log, int task)
{
    BaseType_t HigherPriorityTaskWoken = pdFALSE;

    switch (task) {
    case CAMERA_RECIEVE_QUEUE:
        return xQueueSendFromISR(camera_recieve_queue, log, &HigherPriorityTaskWoken);

    case CAMERA_DISPLAY_QUEUE:
        return xQueueSend(camera_dispaly_queue, log, 0);

    case CAMERA_ALGO_QUEUE:
        return xQueueSend(camera_algo_queue, log, 0);

    case ALGO_DISPLAY_QUEUE:
        return xQueueSend(algo_display_queue, log, 0);

    case ALGO_CODEC_QUEUE:
        return xQueueSend(algo_codec_queue, log, 0);

    case CAMERA_PXP_QUEUE:
        return xQueueSendFromISR(camera_pxp_queue, log, &HigherPriorityTaskWoken);
    }
}

BaseType_t UVITA_MessageRecieve(void* const pvBuffer, int task)
{
    BaseType_t HigherPriorityTaskWoken = pdFALSE;

    switch (task) {
    case CAMERA_RECIEVE_QUEUE:
        return xQueueReceive(camera_recieve_queue, pvBuffer, portMAX_DELAY);
        break;

    case CAMERA_DISPLAY_QUEUE:
        return xQueueReceive(camera_dispaly_queue, pvBuffer, portMAX_DELAY);
        break;

    case CAMERA_ALGO_QUEUE:
        return xQueueReceive(camera_algo_queue, pvBuffer, portMAX_DELAY);
        break;

    case CAMERA_PXP_QUEUE:
        return xQueueReceive(camera_pxp_queue, pvBuffer, portMAX_DELAY);
        break;

    case ALGO_CODEC_QUEUE:
        return xQueueReceive(algo_codec_queue, pvBuffer, portMAX_DELAY);
        break;

    default:
        break;
    }
}

/*!
 * @brief log_init function
 */
void UVITA_MessageInit(uint32_t queue_length, uint32_t max_log_lenght)
{
    camera_recieve_queue = xQueueCreate(queue_length, max_log_lenght);
    camera_dispaly_queue = xQueueCreate(queue_length, max_log_lenght);
    camera_algo_queue = xQueueCreate(queue_length, max_log_lenght);
    algo_display_queue = xQueueCreate(queue_length, max_log_lenght);
    algo_codec_queue = xQueueCreate(queue_length, max_log_lenght);
    camera_pxp_queue = xQueueCreate(queue_length, max_log_lenght);

    /* Enable queue view in MCUX IDE FreeRTOS TAD plugin. */
    if (camera_recieve_queue != NULL) {
        vQueueAddToRegistry(camera_recieve_queue, "camera_recieve");
    }

    if (camera_dispaly_queue != NULL) {
        vQueueAddToRegistry(camera_dispaly_queue, "camera_dispaly");
    }

    if (camera_algo_queue != NULL) {
        vQueueAddToRegistry(camera_algo_queue, "camera_algo");
    }

    if (algo_display_queue != NULL) {
        vQueueAddToRegistry(algo_display_queue, "algo_display");
    }

    if (algo_codec_queue != NULL) {
        vQueueAddToRegistry(algo_display_queue, "algo_codec");
    }

    if (camera_pxp_queue != NULL) {
        vQueueAddToRegistry(camera_pxp_queue, "camera_pxp");
    }

}

