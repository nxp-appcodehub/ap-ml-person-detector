/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "uvita_pxp.h"
#include "uvita_camera.h"

static int PXP_Send = 0;
static int PXP_Recieve = 0;

void PXP_IRQHandler(void)
{
    PXP_ClearStatusFlags(DEMO_PXP, kPXP_CompleteFlag);
    UVITA_MessageAdd(&PXP_Send, CAMERA_PXP_QUEUE);
}

void UVITA_DEMO_InitPxp(void)
{
    /*
     * Configure the PXP for rotate and scale.
     */
    PXP_Init(DEMO_PXP);

    PXP_EnableInterrupts(DEMO_PXP, kPXP_CompleteInterruptEnable);
    EnableIRQ(PXP_IRQn);
    NVIC_SetPriority(PXP_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);

    PXP_SetProcessSurfaceBackGroundColor(DEMO_PXP, 0U);

    /* Disable AS. */
    PXP_SetAlphaSurfacePosition(DEMO_PXP, 0xFFFFU, 0xFFFFU, 0U, 0U);

    PXP_SetCsc1Mode(DEMO_PXP, kPXP_Csc1YCbCr2RGB);

    PXP_EnableCsc1(DEMO_PXP, true);

    PXP_ClearStatusFlags(DEMO_PXP, kPXP_CompleteFlag);
}

void UVITA_PXP_Image565Set(pxp_ps_buffer_config_t* psBufferConfig,
                           pxp_output_buffer_config_t* outputBufferConfig)
{

    outputBufferConfig->pitchBytes     = DEMO_CAMERA_WIDTH * 2;

    outputBufferConfig->width  = DEMO_CAMERA_WIDTH;
    outputBufferConfig->height = DEMO_CAMERA_HEIGHT;

    PXP_SetProcessSurfaceBackGroundColor(DEMO_PXP, 0U);

    PXP_SetProcessSurfacePosition(DEMO_PXP, 0U, 0U, outputBufferConfig->width - 1U, outputBufferConfig->height - 1U);

    PXP_SetRotateConfig(DEMO_PXP, kPXP_RotateOutputBuffer, kPXP_Rotate0, kPXP_FlipHorizontal);

    PXP_SetProcessSurfaceScaler(DEMO_PXP, DEMO_CAMERA_WIDTH, DEMO_CAMERA_HEIGHT, outputBufferConfig->width, outputBufferConfig->height);
}

void UVITA_PXP_LCDSet(pxp_ps_buffer_config_t* psBufferConfig,
                      pxp_output_buffer_config_t* outputBufferConfig)
{

    outputBufferConfig->pitchBytes     = DEMO_BUFFER_WIDTH * 2;

    outputBufferConfig->width  = DEMO_BUFFER_HEIGHT;
    outputBufferConfig->height = DEMO_BUFFER_WIDTH;

    PXP_SetProcessSurfaceBackGroundColor(DEMO_PXP, 0U);

    PXP_SetProcessSurfacePosition(DEMO_PXP, 0U, 0U, DEMO_BUFFER_HEIGHT - 1U, DEMO_BUFFER_WIDTH - 1U);

    PXP_SetRotateConfig(DEMO_PXP, kPXP_RotateOutputBuffer, kPXP_Rotate270, kPXP_FlipDisable);

    PXP_SetProcessSurfaceScaler(DEMO_PXP, DEMO_CAMERA_WIDTH, DEMO_CAMERA_HEIGHT, DEMO_BUFFER_HEIGHT, DEMO_BUFFER_WIDTH);

}

void UVITA_PXP_AlgoSet(pxp_ps_buffer_config_t* psBufferConfig,
                       pxp_output_buffer_config_t* outputBufferConfig)
{

    outputBufferConfig->pitchBytes     = outputBufferConfig->width * 3;

    PXP_SetProcessSurfaceBackGroundColor(DEMO_PXP, 0U);

    PXP_SetProcessSurfacePosition(DEMO_PXP, 0U, 0U, outputBufferConfig->width - 1U, outputBufferConfig->height - 1U);

    PXP_SetRotateConfig(DEMO_PXP, kPXP_RotateOutputBuffer, kPXP_Rotate0, kPXP_FlipHorizontal);

    PXP_SetProcessSurfaceScaler(DEMO_PXP, DEMO_CAMERA_WIDTH, DEMO_CAMERA_HEIGHT, outputBufferConfig->width, outputBufferConfig->height);

}

void UVITA_PXP_CameraPre(pxp_ps_buffer_config_t* psBufferConfig,
                         pxp_output_buffer_config_t* outputBufferConfig,
                         pxp_output_pixel_format_t pixelFormat)
{
    psBufferConfig->pixelFormat = kPXP_PsPixelFormatYUV1P444; /* Note: This is 32-bit per pixel */
    psBufferConfig->swapByte    = false;
    psBufferConfig->bufferAddrU = 0U;
    psBufferConfig->bufferAddrV = 0U;
    psBufferConfig->pitchBytes  = DEMO_CAMERA_WIDTH * DEMO_CAMERA_BUFFER_BPP;

    outputBufferConfig->pixelFormat    = pixelFormat;
    outputBufferConfig->interlacedMode = kPXP_OutputProgressive;
    outputBufferConfig->buffer1Addr    = 0U;

    PXP_SetProcessSurfaceBackGroundColor(DEMO_PXP, 0);
}

void UVITA_PXP_ImageRotatePre(pxp_ps_buffer_config_t* psBufferConfig,
                              pxp_output_buffer_config_t* outputBufferConfig,
                              pxp_output_pixel_format_t pixelFormat)
{
    psBufferConfig->pixelFormat = kPXP_PsPixelFormatRGB565; /* Note: This is 32-bit per pixel */
    psBufferConfig->swapByte    = false;
    psBufferConfig->bufferAddrU = 0U;
    psBufferConfig->bufferAddrV = 0U;
    psBufferConfig->pitchBytes  = DEMO_CAMERA_WIDTH * DEMO_LCD_BUFFER_BPP;

    outputBufferConfig->pixelFormat    = pixelFormat;
    outputBufferConfig->interlacedMode = kPXP_OutputProgressive;
    outputBufferConfig->buffer1Addr    = 0U;

    PXP_SetProcessSurfaceBackGroundColor(DEMO_PXP, 0);

}

void UVITA_PXP_ConvertOut(pxp_ps_buffer_config_t* psBufferConfig,
                          pxp_output_buffer_config_t* outputBufferConfig,
                          void* Output_Buffer_Addr)
{

    //psBufferConfig->bufferAddr = cameraReceivedFrameAddr;
    /* Convert the camera input picture to RGB format. */
    PXP_SetProcessSurfaceBufferConfig(DEMO_PXP, psBufferConfig);

    outputBufferConfig->buffer0Addr = (uint32_t)Output_Buffer_Addr;
    PXP_SetOutputBufferConfig(DEMO_PXP, outputBufferConfig);

    PXP_Start(DEMO_PXP);

    if (UVITA_MessageRecieve(&PXP_Recieve, CAMERA_PXP_QUEUE) != pdTRUE) {
        PRINTF("fail to receive camera pxp queue\r\n");
    }

}

