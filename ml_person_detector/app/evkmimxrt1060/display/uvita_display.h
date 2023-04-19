/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UVITA_DISPLAY_H_
#define _UVITA_DISPLAY_H_

#include "fsl_pxp.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "display_support.h"


/* LCD input frame buffer is RGB565, converted by PXP. */
#define DEMO_LCD_BUFFER_BPP 2
#define DEMO_LCD_BUFFER_COUNT 2

#ifdef __cplusplus
extern    "C"
{
#endif
/*******************************************************************************
 * Prototypes
// ******************************************************************************/

void BOARD_ResetDisplayMix(void);
void UVITA_DEMO_BufferSwitchOffCallback(void* param, void* switchOffBuffer);
void UVITA_DEMO_InitDisplay(void);
void* UVITA_DEMO_LCDBufferAddr(void);
void UVITA_DEMO_DisplayRun(void);

#ifdef __cplusplus
}
#endif

#endif /*_UVITA_DISPLAY_H_*/
