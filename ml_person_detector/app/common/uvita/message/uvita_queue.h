/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UVITA_QUEUE_H_
#define _UVITA_QUEUE_H_

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "fsl_debug_console.h"

typedef enum {
    CAMERA_RECIEVE_QUEUE,
    CAMERA_DISPLAY_QUEUE,
    CAMERA_ALGO_QUEUE,
    ALGO_DISPLAY_QUEUE,
    ALGO_CODEC_QUEUE,
    CAMERA_PXP_QUEUE
} TASKS_QUEUE;


#ifdef __cplusplus
extern    "C"
{
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* Logger API */
void UVITA_MessageInit(uint32_t queue_length, uint32_t max_log_lenght);
BaseType_t UVITA_MessageAdd(int* log, int task);
BaseType_t UVITA_MessageRecieve(void* const pvBuffer, int task);

#ifdef __cplusplus
}
#endif

#endif /*_SOLUTIONS_QUEUE_H_*/
