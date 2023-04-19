/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "uvita_algo.h"
#include "uvita_nms.h"
/**
 * @brief compute the overlap between two boxes
 *
 * @param[in]      box_a             detection box
 * @param[in]      box_b             detection box
 *
 * @return overlap ratio
 *
 */
float nms_overlap(DetBox_t box_a, DetBox_t box_b)
{
    float x1 = UVITA_MAX(box_a.rect[RECT_LEFT_X], box_b.rect[RECT_LEFT_X]);
	float y1 = UVITA_MAX(box_a.rect[RECT_TOP_Y], box_b.rect[RECT_TOP_Y]);
	float x2 = UVITA_MIN(box_a.rect[RECT_RIGHT_X], box_b.rect[RECT_RIGHT_X]);
	float y2 = UVITA_MIN(box_a.rect[RECT_BOTTOM_Y], box_b.rect[RECT_BOTTOM_Y]);

	float w = x2 - x1;
	float h = y2 - y1;

	float intersection = w * h;

	float aarea = (box_a.rect[RECT_RIGHT_X] - box_a.rect[RECT_LEFT_X]) * (box_a.rect[RECT_BOTTOM_Y] - box_a.rect[RECT_TOP_Y]);
	float barea = (box_b.rect[RECT_RIGHT_X] - box_b.rect[RECT_LEFT_X]) * (box_b.rect[RECT_BOTTOM_Y] - box_b.rect[RECT_TOP_Y]);

	float iou = intersection / (aarea + barea - intersection);
	return (w <= 0 || h <= 0) ? 0.0f : iou;
}

int nms_max_score(DetBox_t* pboxes, int box_num)
{
	float max_score = 0.0f;
	int max_score_index;
	for(int i=0; i<box_num; i++) {
		if (pboxes[i].score > max_score) {
			max_score = pboxes[i].score;
			max_score_index = i;
		}
	}
	return max_score_index;
}

/**
 * @brief uvita NMS function
 *
 * @param[in]      pboxes       candidate boxes pointer
 * @param[in]      box_num      the number of the candidate boxes
 *
 * @return         number of boxes after NMS
 *
 */
int uvita_nms(DetBox_t* pboxes, int box_num)
{
    if (box_num == 0) {
        return box_num;
	}
    int candidate_num = box_num;
    DetBox_t* candidateboxes = (DetBox_t*)malloc(box_num*sizeof(DetBox_t));
    memcpy(candidateboxes, pboxes, box_num*sizeof(DetBox_t));

    box_num = 0;

    while (candidate_num)
    {
    	int index = nms_max_score(candidateboxes, candidate_num);
    	memcpy(&pboxes[box_num], &candidateboxes[index], sizeof(DetBox_t));
    	int tmp_num = 0;
        for(int i=0; i<candidate_num; i++) {
        	if (i != index && nms_overlap(pboxes[box_num], candidateboxes[i]) < NMS_THRESHOLD) {
        		memmove(&candidateboxes[tmp_num], &candidateboxes[i], sizeof(DetBox_t));
        		tmp_num++;
        	}
        }
        candidate_num = tmp_num;
        box_num++;
    }

    free(candidateboxes);

	return box_num;
}
