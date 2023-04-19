/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef UVITA_DPERSON_H_
#define UVITA_DPERSON_H_

#include <stdint.h>
#include "uvita_algo.h"
#include "uvita_types.h"

#define DPERSON_MODEL_INPUT_H 192
#define DPERSON_MODEL_INPUT_W 320
#define DPERSON_MODEL_INPUT_C 3

#define DPERSON_MODEL_OUTPUT_H DPERSON_MODEL_INPUT_H/16
#define DPERSON_MODEL_OUTPUT_W DPERSON_MODEL_INPUT_W/16
#define DPERSON_MODEL_OUTPUT_C 6

#define DPERSON_MODEL_INPUT_MEAN 0
#define DPERSON_MODEL_INPUT_SCALE 0.0039216f

#define DPERSON_THRESHOLD 0.85f

#ifdef __cplusplus
extern    "C"
{
#endif

/**
 * @brief multiple person detection
 *
 * @param[in]      frame             frame for detection
 * @param[out]     person_out        output person bounding box
 *
 * @return         function status
 *
 */
uvita_status uvita_dperson_run(uvita_image_frame* frame,
                               uvita_person_out* pout);

#ifdef __cplusplus
}
#endif

#endif /* UVITA_DPERSON_H_ */
