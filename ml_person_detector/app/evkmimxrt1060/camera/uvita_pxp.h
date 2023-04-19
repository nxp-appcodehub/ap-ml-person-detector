/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_pxp.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "display_support.h"
#include "camera_support.h"

#include "uvita_display.h"


#ifndef _UVITA_PXP_H_
#define _UVITA_PXP_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_PXP PXP

#ifdef __cplusplus
extern    "C"
{
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void UVITA_DEMO_InitPxp(void);
void UVITA_PXP_LCDSet(pxp_ps_buffer_config_t* psBufferConfig,
                      pxp_output_buffer_config_t* outputBufferConfig);
void UVITA_PXP_AlgoSet(pxp_ps_buffer_config_t* psBufferConfig,
                       pxp_output_buffer_config_t* outputBufferConfig);
void UVITA_PXP_CameraPre(pxp_ps_buffer_config_t* psBufferConfig,
                         pxp_output_buffer_config_t* outputBufferConfig,
                         pxp_output_pixel_format_t pixelFormat);
void UVITA_PXP_ConvertOut(pxp_ps_buffer_config_t* psBufferConfig,
                          pxp_output_buffer_config_t* outputBufferConfig,
                          void* Output_Buffer_Addr);

#ifdef __cplusplus
}
#endif

#endif /*_SLN_PXP_H_*/
