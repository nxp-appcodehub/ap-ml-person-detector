/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "uvita_algo.h"
#include "uvita_camera.h"
#include "uvita_pxp.h"
#include "uvita_queue.h"
/*
 * When new frame buffer sent to display, it might not be shown immediately.
 * Application could use callback to get new frame shown notification, at the
 * same time, when this flag is set, application could write to the older
 * frame buffer.
 */

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile bool s_newFrameShown = false;
static dc_fb_info_t fbInfo;
static volatile uint8_t s_lcdActiveFbIdx;

AT_NONCACHEABLE_SECTION_ALIGN(
    uint8_t s_lcdBuffer[DEMO_LCD_BUFFER_COUNT][DEMO_BUFFER_HEIGHT][DEMO_BUFFER_WIDTH * DEMO_LCD_BUFFER_BPP],
    FRAME_BUFFER_ALIGN);
uvita_image_frame displayFrame = {DEMO_BUFFER_HEIGHT, DEMO_BUFFER_WIDTH, UVITA_IMG_INT16_RGB565};
/*******************************************************************************
 * Code
 ******************************************************************************/

void UVITA_DEMO_InitDisplay(void)
{
    status_t status;

    BOARD_PrepareDisplayController();

    status = g_dc.ops->init(&g_dc);

    if (kStatus_Success != status) {
        PRINTF("Display initialization failed\r\n");
        assert(0);
    }

    g_dc.ops->getLayerDefaultConfig(&g_dc, 0, &fbInfo);
    fbInfo.pixelFormat = kVIDEO_PixelFormatRGB565;
    fbInfo.width       = DEMO_BUFFER_WIDTH;
    fbInfo.height      = DEMO_BUFFER_HEIGHT;
    fbInfo.startX      = DEMO_BUFFER_START_X;
    fbInfo.startY      = DEMO_BUFFER_START_Y;
    fbInfo.strideBytes = DEMO_BUFFER_WIDTH * DEMO_LCD_BUFFER_BPP;
    g_dc.ops->setLayerConfig(&g_dc, 0, &fbInfo);

    g_dc.ops->setCallback(&g_dc, 0, UVITA_DEMO_BufferSwitchOffCallback, NULL);

    s_lcdActiveFbIdx = 0;
    s_newFrameShown  = false;
    g_dc.ops->setFrameBuffer(&g_dc, 0, s_lcdBuffer[s_lcdActiveFbIdx]);

    /* For the DBI interface display, application must wait for the first
     * frame buffer sent to the panel.
     */
    if ((g_dc.ops->getProperty(&g_dc) & kDC_FB_ReserveFrameBuffer) == 0) {
        while (s_newFrameShown == false) {
        }
    }

    s_newFrameShown = true;

    g_dc.ops->enableLayer(&g_dc, 0);

}

void UVITA_DEMO_BufferSwitchOffCallback(void* param, void* switchOffBuffer)
{
    s_newFrameShown = true;
}

void* UVITA_DEMO_LCDBufferAddr(void)
{
    return s_lcdBuffer[s_lcdActiveFbIdx ^ 1];
}

/*!
 * @brief lcd display function
 */
void UVITA_DEMO_DisplayRun(void)
{
    int lcdShown;

    while (1) {
        if (UVITA_MessageRecieve(&lcdShown, CAMERA_DISPLAY_QUEUE) != pdTRUE) {
            PRINTF("fail to receive new frame queue\r\n");
        }

        s_newFrameShown = false;
        /* update shown frame with algorithm results */
        displayFrame.data = (uint8_t*)s_lcdBuffer[s_lcdActiveFbIdx ^ 1];
        UVITA_DEMO_AlgoGUI(&displayFrame);

        g_dc.ops->setFrameBuffer(&g_dc, 0, s_lcdBuffer[s_lcdActiveFbIdx ^ 1]);
        s_lcdActiveFbIdx ^= 1;
    }
}
