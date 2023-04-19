/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef UVITA_PRESENT_H_
#define UVITA_PRESENT_H_

#include <stdint.h>
#include <string.h>
#include "uvita_types.h"

#define UVITA_RGB565_RED            0xF100U
#define UVITA_RGB565_BLUE           0x001FU
#define UVITA_RGB565_GREEN          0x07E0U
#define UVITA_RGB565_BLACK          0x0000U
#define UVITA_RGB565_WHITE          0xFFFFU
#define UVITA_RGB565_YELLOW         (UVITA_RGB565_RED | UVITA_RGB565_GREEN)

typedef struct {
  unsigned short XSize;
  unsigned short YSize;
  unsigned short BytesPerLine;
  unsigned short BitsPerPixel;
  const unsigned char * pData;
} GUI_BITMAP;

#ifdef __cplusplus
extern "C" {
#endif

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
                        uvita_present_color color);
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
                       uvita_present_color color);

/**
 * @brief uvita presentation for nxp logo on RGB565 frame
 *
 * @param[in,out]  frame           frame information
 *
 * @return         none
 *
 */
void uvita_present_nxp_logo(uvita_image_frame* frame);

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
                        uvita_present_color color);

#ifdef __cplusplus
}
#endif

#endif
