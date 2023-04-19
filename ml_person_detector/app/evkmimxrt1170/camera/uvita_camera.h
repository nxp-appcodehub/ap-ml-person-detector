/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UVITA_CAMERA_H_
#define _UVITA_CAMERA_H_

#include "fsl_pxp.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "display_support.h"
#include "camera_support.h"

#include "uvita_pxp.h"
#include "uvita_algo.h"
#include "uvita_display.h"
#include "uvita_queue.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* CSI output frame buffer is XYUV8888. */
#define DEMO_CAMERA_BUFFER_BPP 4

#define DEMO_CAMERA_BUFFER_COUNT 2

#if (((DEMO_CAMERA_WIDTH < DEMO_CAMERA_HEIGHT) && (DEMO_BUFFER_WIDTH > DEMO_BUFFER_HEIGHT)) || \
     ((DEMO_CAMERA_WIDTH > DEMO_CAMERA_HEIGHT) && (DEMO_BUFFER_WIDTH < DEMO_BUFFER_HEIGHT)))
#define DEMO_ROTATE_FRAME 1
#else
#define DEMO_ROTATE_FRAME 0
#endif


#ifdef __cplusplus
extern    "C"
{
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void UVITA_DEMO_InitCamera(void);
void UVITA_DEMO_CameraRun(void);

#ifdef __cplusplus
}
#endif

#endif /*_SOLUTIONS_CAMERA_H_*/
