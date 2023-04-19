/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_soc_src.h"
#include "uvita_camera.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* pxp parameter for converting camera to image565 */
pxp_ps_buffer_config_t psImage565Config;
pxp_output_buffer_config_t outputImage565Config;
/* pxp parameter for converting image565 to display */
pxp_ps_buffer_config_t psDisplayConfig;
pxp_output_buffer_config_t outputDisplayConfig;
/* pxp parameter for converting camera to algorithm */
pxp_ps_buffer_config_t psAlgoConfig;
pxp_output_buffer_config_t outputAlgoConfig;

AT_NONCACHEABLE_SECTION_ALIGN(
    static uint8_t s_cameraBuffer[DEMO_CAMERA_BUFFER_COUNT][DEMO_CAMERA_HEIGHT][DEMO_CAMERA_WIDTH * DEMO_CAMERA_BUFFER_BPP],
    DEMO_CAMERA_BUFFER_ALIGN);
AT_NONCACHEABLE_SECTION_ALIGN(
    uint8_t s_image565Buffer[DEMO_CAMERA_HEIGHT][DEMO_CAMERA_WIDTH * DEMO_LCD_BUFFER_BPP],
    FRAME_BUFFER_ALIGN);

/* message sent or received from camera */
static int mCamera = 0;
static int rCamera = 1;
/*
 * When new frame buffer sent to display, it might not be shown immediately.
 * Application could use callback to get new frame shown notification, at the
 * same time, when this flag is set, application could write to the older
 * frame buffer.
 */

//static void DEMO_BufferSwitchOffCallback(void *param, void *switchOffBuffer);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 * camera dev callback
 * camera_receiver_handle_t *handle, status_t status, void *userData
 */
void UVITA_CAMERA_DevCallback(camera_receiver_handle_t* handle, status_t status, void* userData)
{
    if (UVITA_MessageAdd(&mCamera, CAMERA_RECIEVE_QUEUE) != pdTRUE) {
        PRINTF("fail to send data from camera\r\n");
    }
}

void UVITA_DEMO_InitCamera(void)
{
    camera_config_t cameraConfig;

    memset(&cameraConfig, 0, sizeof(cameraConfig));

    BOARD_InitCameraResource();

    /* CSI input data bus is 24-bit, and save as XYUV8888.. */
    cameraConfig.pixelFormat                = kVIDEO_PixelFormatXYUV;
    cameraConfig.bytesPerPixel              = DEMO_CAMERA_BUFFER_BPP;
    cameraConfig.resolution                 = FSL_VIDEO_RESOLUTION(DEMO_CAMERA_WIDTH, DEMO_CAMERA_HEIGHT);
    cameraConfig.frameBufferLinePitch_Bytes = DEMO_CAMERA_WIDTH * DEMO_CAMERA_BUFFER_BPP;
    cameraConfig.interface                  = kCAMERA_InterfaceGatedClock;
    cameraConfig.controlFlags               = DEMO_CAMERA_CONTROL_FLAGS;
    cameraConfig.framePerSec                = DEMO_CAMERA_FRAME_RATE;

    NVIC_SetPriority(CSI_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
    CAMERA_RECEIVER_Init(&cameraReceiver, &cameraConfig, UVITA_CAMERA_DevCallback, NULL);

    BOARD_InitMipiCsi();

    cameraConfig.pixelFormat   = kVIDEO_PixelFormatYUYV;
    cameraConfig.bytesPerPixel = 2;
    cameraConfig.resolution    = FSL_VIDEO_RESOLUTION(DEMO_CAMERA_WIDTH, DEMO_CAMERA_HEIGHT);
    cameraConfig.interface     = kCAMERA_InterfaceMIPI;
    cameraConfig.controlFlags  = DEMO_CAMERA_CONTROL_FLAGS;
    cameraConfig.framePerSec   = DEMO_CAMERA_FRAME_RATE;
    cameraConfig.csiLanes      = DEMO_CAMERA_MIPI_CSI_LANE;
    CAMERA_DEVICE_Init(&cameraDevice, &cameraConfig);

    CAMERA_DEVICE_Start(&cameraDevice);

    /* Submit the empty frame buffers to buffer queue. */
    for (uint32_t i = 0; i < DEMO_CAMERA_BUFFER_COUNT; i++) {
        CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)(s_cameraBuffer[i]));
    }
}


void UVITA_DEMO_CameraRun(void)
{
    uint32_t cameraReceivedFrameAddr;
    uint8_t* detFrameAddr;

    UVITA_PXP_CameraPre(&psImage565Config, &outputImage565Config, kPXP_OutputPixelFormatRGB565);
    UVITA_PXP_CameraPre(&psAlgoConfig, &outputAlgoConfig, kPXP_OutputPixelFormatRGB888P);
    UVITA_PXP_ImageRotatePre(&psDisplayConfig, &outputDisplayConfig, kPXP_OutputPixelFormatRGB565);

    CAMERA_RECEIVER_Start(&cameraReceiver);

    while (1) {
        if (UVITA_MessageRecieve(&rCamera, CAMERA_RECIEVE_QUEUE) != pdTRUE) {
            PRINTF("fail to receive from camera\r\n");
        }

        /* Wait to get the full frame buffer to show. */
        while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &cameraReceivedFrameAddr)) {
            PRINTF("kStatus_Success \r\n");
        }

#if UVITA_DISPLAY_ENABLE
        uvita_image_frame displayFrame = {DEMO_CAMERA_HEIGHT, DEMO_CAMERA_WIDTH, UVITA_IMG_INT16_RGB565, (uint8_t*)s_image565Buffer};
        /* convert camera YUV frame to rgb565 image */
        psImage565Config.bufferAddr = cameraReceivedFrameAddr;
        UVITA_PXP_Image565Set(&psImage565Config, &outputImage565Config);
        UVITA_PXP_ConvertOut(&psImage565Config, &outputImage565Config, s_image565Buffer);
        /* update algorithm results */
        UVITA_DEMO_AlgoGUI(&displayFrame);
        /* rotate rgb565 image to display */
        psDisplayConfig.bufferAddr = (uint32_t)s_image565Buffer;
        PXP_EnableCsc1(DEMO_PXP, false);
        UVITA_PXP_LCDSet(&psDisplayConfig, &outputDisplayConfig);
        UVITA_PXP_ConvertOut(&psDisplayConfig, &outputDisplayConfig, UVITA_DEMO_LCDBufferAddr());
        PXP_EnableCsc1(DEMO_PXP, true);

        /* Show the new frame. */
        if (UVITA_MessageAdd(&mCamera, CAMERA_DISPLAY_QUEUE) != pdTRUE) {
            PRINTF("fail to message from camera to display\r\n");
        }

#endif
        /* convert camera to algorithm buffer */
        UVITA_DEMO_FrameAddr(&detFrameAddr);
        if (detFrameAddr != NULL) {
            psAlgoConfig.bufferAddr = cameraReceivedFrameAddr;
            outputAlgoConfig.width  = UVITA_DET_FRAME_W;
            outputAlgoConfig.height = UVITA_DET_FRAME_H;
            UVITA_PXP_AlgoSet(&psAlgoConfig, &outputAlgoConfig);
            UVITA_PXP_ConvertOut(&psAlgoConfig, &outputAlgoConfig, (void*)detFrameAddr);

            if (UVITA_MessageAdd(&mCamera, CAMERA_ALGO_QUEUE) != pdTRUE) {
                PRINTF("fail to message from camera to algo\r\n");
            }
        }

        /* Return the camera buffer to camera receiver handle. */
        CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, (uint32_t)cameraReceivedFrameAddr);
    }
}
