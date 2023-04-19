/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef UVITA_ALGO_H_
#define UVITA_ALGO_H_

#include <stdint.h>
#include "uvita_types.h"
#include "uvita_dperson.h"

#define UVITA_FULL_FRAME_H DEMO_CAMERA_HEIGHT
#define UVITA_FULL_FRAME_W DEMO_CAMERA_WIDTH

#define UVITA_DET_FRAME_H DPERSON_MODEL_INPUT_H
#define UVITA_DET_FRAME_W DPERSON_MODEL_INPUT_W

#ifdef __cplusplus
extern    "C"
{
#endif
/*******************************************************************************
 * Prototypes
// ******************************************************************************/

void UVITA_DEMO_Print(char* str);
void UVITA_DEMO_AlgoInit(void);
void UVITA_DEMO_FrameAddr(uint8_t** detBuffer);
void UVITA_DEMO_AlgoGUI(uvita_image_frame* frame);
void UVITA_DEMO_AlgoRun(void);
/**
 * @brief Bare metal speed ​​test for multiple person detection
 */
void uvita_bmtest(void);

#ifdef __cplusplus
}
#endif

#endif /* UVITA_ALGO_H_ */
