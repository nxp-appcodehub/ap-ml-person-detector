/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <string.h>

#include "../presentation/data/nxp_rgb565_128_45.h"
#include "../presentation/data/nxp_rgb565_240_86.h"
#include "uvita_types.h"


uint16_t color_list[UVITA_COLOR_NUM] = {
    0xF100U,                  /**< UVITA present red color for RGB565 */
    0x001FU,                  /**< UVITA present blue color for RGB565 */
    0x07E0U,                  /**< UVITA present green color for RGB565 */
    0xF7E0U,                  /**< UVITA present yellow color for RGB565 */
    0x0000U,                  /**< UVITA present black color for RGB565 */
    0xFFFFU,                  /**< UVITA present white color for RGB565 */
};

void uvita_present_point(uvita_image_frame* frame,
                         int points_x,
                         int points_y,
                         int radius,
                         uvita_present_color color)
{
    uint16_t* buf = (uint16_t*)(frame->data);

    points_x = UVITA_MAX(radius, points_x);
    points_y = UVITA_MAX(radius, points_y);
    points_x = UVITA_MIN(frame->width - 1 - radius, points_x);
    points_y = UVITA_MIN(frame->height - 1 - radius, points_y);

    for (int x = points_x - radius; x <= points_x + radius; x++) {
        for (int y = points_y - radius; y <= points_y + radius; y++) {
            if ((x - points_x) * (x - points_x) + (y - points_y) * (y - points_y) <= radius * radius) {
                *(buf + y * frame->width + x) = color_list[color];
            }
        }
    }
}
/**
 * @brief uvita presentation for drawing line on RGB565 frame
 *
 * @param[in,out]  frame            frame information
 * @param[in]      left_x           start point at x-axis
 * @param[in]      top_y            start point at y-axis
 * @param[in]      right_x          end point at x-axis
 * @param[in]      bottom_y         end point at y-axis
 * @param[in]      color            the color of the line
 *
 * @return         none
 *
 */
void uvita_present_line(uvita_image_frame* frame,
                        int left_x,
                        int top_y,
                        int right_x,
                        int bottom_y,
                        uvita_present_color color)
{
    left_x = UVITA_MAX(0, left_x);
    top_y = UVITA_MAX(0, top_y);
    right_x = UVITA_MIN(frame->width - 1, right_x);
    bottom_y = UVITA_MIN(frame->height - 1, bottom_y);
    uint16_t* buf = (uint16_t*)(frame->data);

    for (int y = top_y; y <= bottom_y; y++) {
        for (int x = left_x; x <= right_x; x++) {
            *(buf + y * frame->width + x) = color_list[color];
        }
    }
}
/**
 * @brief uvita presentation for drawing box on RGB565 frame
 *
 * @param[in,out]  frame            frame information
 * @param[in]      left_x           start point at x-axis
 * @param[in]      top_y            start point at y-axis
 * @param[in]      right_x          end point at x-axis
 * @param[in]      bottom_y         end point at y-axis
 * @param[in]      line_width       width of the box line
 * @param[in]      color            the color of the line
 *
 * @return         none
 *
 */
void uvita_present_box(uvita_image_frame* frame,
                       int left_x,
                       int top_y,
                       int right_x,
                       int bottom_y,
                       int line_width,
                       uvita_present_color color)
{
    int start_x, start_y, end_x, end_y;
    /* draw top line */
    start_y = top_y;
    end_y = start_y + line_width;
    uvita_present_line(frame, left_x, start_y, right_x, end_y, color);
    /* draw bottom line */
    start_y = bottom_y;
    end_y = bottom_y + line_width;
    uvita_present_line(frame, left_x, start_y, right_x, end_y, color);
    /* draw left line */
    start_x = left_x;
    end_x = left_x + line_width;
    uvita_present_line(frame, start_x, top_y, end_x, bottom_y, color);
    /* draw right line */
    start_x = right_x;
    end_x = right_x + line_width;
    uvita_present_line(frame, start_x, top_y, end_x, bottom_y, color);
}

/**
 * @brief uvita presentation for nxp logo on RGB565 frame
 *
 * @param[in,out]  frame           frame information
 *
 * @return         none
 *
 */
void uvita_present_nxp_logo(uvita_image_frame* frame)
{
    GUI_CONST_STORAGE GUI_BITMAP* nxp_logo;

    if (frame->height >= 480 && frame->width >= 640) {
        nxp_logo = &bmnxp_rgb565_240_86;
    } else {
        nxp_logo = &bmnxp_rgb565_128_45;
    }

    int left_x = frame->width - nxp_logo->XSize;
    int top_y = frame->height - nxp_logo->YSize;
    uint16_t* frameBuf = (uint16_t*)(frame->data);
    uint16_t* nxpBuf = (uint16_t*)(nxp_logo->pData);
    uint16_t  tmpData;

    for (int y = top_y; y < UVITA_MIN(top_y + nxp_logo->YSize - 1, frame->height); y++) {
        for (int x = left_x; x < UVITA_MIN(left_x + nxp_logo->XSize - 1, frame->width); x++) {
            tmpData = *(nxpBuf + (y - top_y) * nxp_logo->XSize + x - left_x);

            if (tmpData != 0x0000) {
                *(frameBuf + y * frame->width + x) = tmpData;
            }
        }
    }
}

/**
 * @brief uvita presentation for drawing person box information on RGB565 frame
 *
 * @param[in,out]  frame            frame information
 * @param[in]      person           person box and/or landmark information
 * @param[in]      line_width       width of the box line
 * @param[in]      color            the color of the line
 *
 * @return         none
 *
 */
void uvita_present_person(uvita_image_frame* frame,
                          uvita_person_out person,
                          int line_width,
                          uvita_present_color color)
{
    for (int i = 0; i < person.has_person; i++) {
        /* person boxes */
        uvita_present_box(frame,
                          person.boxes[i].rect[RECT_LEFT_X],
                          person.boxes[i].rect[RECT_TOP_Y],
                          person.boxes[i].rect[RECT_RIGHT_X],
                          person.boxes[i].rect[RECT_BOTTOM_Y],
                          line_width,
                          color);
    }

}
