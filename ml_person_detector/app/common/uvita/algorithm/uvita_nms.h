/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef UVITA_NMS_H_
#define UVITA_NMS_H_

#include "uvita_algo.h"

#ifndef UVITA_MAX
#define UVITA_MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif
#ifndef UVITA_MIN
#define UVITA_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define NMS_THRESHOLD 0.35f

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief compute the overlap between two boxes
 *
 * @param[in]      box_a             detection box
 * @param[in]      box_b             detection box
 *
 * @return overlap ratio
 *
 */
float nms_overlap(DetBox_t box_a, DetBox_t box_b);
/**
 * @brief uvita NMS function
 *
 * @param[in]      pboxes       candidate boxes pointer
 * @param[in]      box_num      the number of the candidate boxes
 *
 * @return         number of boxes after NMS
 *
 */
int uvita_nms(DetBox_t* pboxes, int box_num);


#ifdef __cplusplus
}
#endif


#endif /* UVITA_NMS_H_ */
