/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef UVITA_TYPES_H_
#define UVITA_TYPES_H_

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define UVITA_MAX_OBJECT_NUM 10

#define UVITA_ROUND(x) ((int)(((x) + 0.5f) - ((x) < 0.0f)))

#ifndef UVITA_MAX
#define UVITA_MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif
#ifndef UVITA_MIN
#define UVITA_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/*
 * @brief the supported input pixel format.
 */
typedef enum {
    UVITA_PRESENT_RED = 0,             /**< UVITA present red color */
    UVITA_PRESENT_BLUE,                /**< UVITA present blue color */
    UVITA_PRESENT_GREEN,               /**< UVITA present green color */
    UVITA_PRESENT_YELLOW,              /**< UVITA present yellow color */
    UVITA_PRESENT_BLACK,               /**< UVITA present black color */
    UVITA_PRESENT_WHITE,               /**< UVITA present white color */
    UVITA_COLOR_NUM,
} uvita_present_color;

/*
 * @brief the supported input pixel format.
 */
typedef enum {
    UVITA_IMG_INT8_RGB888 = 0,        /**< RGB888 from low address to high */
    UVITA_IMG_INT8_BGR888,            /**< BGR888 from low address to high */
    UVITA_IMG_INT8_GRAY,              /**< Gray8 */
    UVITA_IMG_FLOAT_RGB,              /**< RGB float format */
    UVITA_IMG_FLOAT_BGR,              /**< BGR float format */
    UVITA_IMG_FLOAT_GRAY,             /**< Gray float format */
    UVITA_IMG_INT16_RGB565,           /**< RGB65 format */
    UVITA_IMG_INT16_BGR565,           /**< BGR565 format */
} uvita_image_format;

/*
 * @brief image frame information
 */
typedef struct {
    uint16_t height;            /**< height */
    uint16_t width;             /**< width */
    uvita_image_format fmt;     /**< input image format */
    uint8_t* data;              /**< buffer with input data */
    uint8_t reserved[3];
} uvita_image_frame;

/*
 * @brief the uvita neural network input.
 */
typedef struct {
    unsigned int c;             /**< input channel */
    unsigned int h;             /**< height */
    unsigned int w;             /**< width */
    uint8_t* buf;               /**< buffer with input data */
    uvita_image_format fmt;     /**< input image format */
    float meanValue;            /**< the mean value in normalization */
    float scaleValue;           /**< the scale value in normalization */
} uvita_nn_input;

/*
 * @brief the uvita neural network output.
 */
typedef struct {
    unsigned int c;             /**< channel */
    unsigned int h;             /**< height */
    unsigned int w;             /**< width */
    float* data;                /**< float data */
    int id;                     /**< network output ID */
    int isFirst;               /**< the first output? */
    int isLast;                /**< the last output? */
    float threshold;            /**< model output threshold */
} uvita_nn_output;

/*
 * @brief uvita neural network engine list.
 */
typedef enum {
    UVITA_NANOAI,               /**< nanoai engine */
    UVITA_GLOW,                 /**< EIQ glow engine */
    UVITA_TENSORFLOW_LITE,      /**< EIQ tensorflow lite engine */
} uvita_nn_engine;

/*
 * @brief rectangle index for left top and right bottom point
 */
typedef enum {
    RECT_LEFT_X,                /**< X-axis of left-top corner*/
    RECT_TOP_Y,                 /**< Y-axis of left-top corner*/
    RECT_RIGHT_X,               /**< X-axis of right-bottom corner*/
    RECT_BOTTOM_Y,              /**< Y-axis of right-bottom corner*/
    RECT_NUM,
} uvita_rect_index;

/*
 * @brief uvita facial box information
 */
typedef struct {
    bool has_face;              /**< whether the face is detected or not; */
    int rect[RECT_NUM];         /**< left, top, right, bottom. */
    float fld[5*2];             /**< 5 landmark point, x0-x4, y0-y4.*/
} uvita_fbox;

/*
 * @brief bounding box information
 */
typedef struct DetBox {
    int rect[RECT_NUM];        /**< left, top, right, bottom */
    float score;               /**< detection score */
    float area;                /**< detection area */
    int reserved;
} DetBox_t;

/*
 * @brief uvita person detection information
 */
typedef struct {
    int has_person;              /**< the number of person */
    DetBox_t boxes[UVITA_MAX_OBJECT_NUM]; /**< person location: left, top, right, bottom. */
} uvita_person_out;

/**
  * @brief Error status returned by some functions in the uvita library.
  */
typedef enum {
    UVITA_SUCCESS                 =  0,        /**< No error */
    UVITA_ARGUMENT_ERROR          =  1,        /**< One or more arguments are incorrect */
    UVITA_MEMORY_ERROR            =  2,        /**< Length of data buffer is incorrect */
    UVITA_UNSUPPORT_ERROR         =  3,        /**< Function is not supported */
    UVITA_NOT_READY               =  4,        /**< Function is not ready but planed to support */
} uvita_status;

#endif
