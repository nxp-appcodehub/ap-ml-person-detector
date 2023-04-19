/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Bundle includes.
#include <string.h>
#include <stdlib.h>
#include "math.h"

#include "dperson_shufflenetv2.h"
#include "glow_bundle_utils.h"
#include "uvita_nms.h"
#include "uvita_algo.h"
#include "uvita_dperson.h"
// ----------------------------- Bundle API -----------------------------

GLOW_MEM_ALIGN(DPERSON_SHUFFLENETV2_MEM_ALIGN)
const uint8_t dperson_constantWeight[DPERSON_SHUFFLENETV2_CONSTANT_MEM_SIZE] = {
    #include "dperson_shufflenetv2.weights.txt"
};

// Statically allocate memory for mutable weights (model input/output data).
GLOW_MEM_ALIGN(DPERSON_SHUFFLENETV2_MEM_ALIGN)
uint8_t dperson_mutableWeight[DPERSON_SHUFFLENETV2_MUTABLE_MEM_SIZE];

// Statically allocate memory for activations (model intermediate results).
GLOW_MEM_ALIGN(DPERSON_SHUFFLENETV2_MEM_ALIGN)
//__attribute__((section(".data.$SRAM_OC1"), aligned(64))) // For i.MX RT1170
//__attribute__((section(".data.$SRAM_OC"), aligned(64))) // For i.MX RT1060
uint8_t dperson_activations[DPERSON_SHUFFLENETV2_ACTIVATIONS_MEM_SIZE];

// Bundle input data absolute address.
uint8_t *dperson_inputAddr = GLOW_GET_ADDR(dperson_mutableWeight, DPERSON_SHUFFLENETV2_input_1);

// Bundle output data absolute address.
uint8_t *dperson_outputAddr = GLOW_GET_ADDR(dperson_mutableWeight, DPERSON_SHUFFLENETV2_A758);

// ---------------------------- Application -----------------------------
// model input data size (bytes).
#define PERSON_INPUT_SIZE    DPERSON_MODEL_INPUT_H*DPERSON_MODEL_INPUT_W*DPERSON_MODEL_INPUT_C

// Allocate buffer for input data. This buffer contains the input image
// pre-processed and serialized as text to include here.

/**
 * @brief A transform from HWC format to CHW format with integer data
 *
 * @param[in]      src      pointer to input tensor
 * @param[out]     dst      pointer to output tensor
 * @param[in]      height   the height of the input tensor
 * @param[in]      width    the width of the input tensor
 * @param[in]      channels the channels of the input tensor
 *
 * @return         0 (success) or -1 (fail)
 *
 */
int trans_hwc2chw(uint8_t* src, uint8_t* dst, int height, int width, int channels)
{
    if (src == NULL || dst == NULL) {
        return -1;
    }

    if (channels == 1) {
        return 0;
    }

    int cw = channels * width;

    // Convert HWC to CHW
    for (int c = 0; c < channels; ++c) {
        for (int h = 0; h < height; ++h) {
            int step = h * cw + c;

            for (int w = 0; w < width; ++w) {
                int index = step + w * channels;
                *dst++ = *(src + index);
            }
        }
    }

    return 0;
}

/*
 * Sigmoid function
 */
float Sigmoid(float x)
{
    return 1.0f / (1.0f + exp(-x));
}

/*
 * Tanh function
 */
float Tanh(float x)
{
    return 2.0f / (1.0f + exp(-2 * x)) - 1;
}

/**
 * @brief post-process of anchor free output
 *
 * @param[in]      input          model input information
 * @param[in]      output         output of object detect model
 * @param[out]     pboxes         the pointer of the bounding boxes
 *
 * @return         the number of bounding boxes
 *
 */
int anchor_free_process(uvita_nn_input input,
                        uvita_nn_output output,
						DetBox_t* pboxes)
{
	int cnt = 0;
	float ratio_w = 352.0f / input.w;
	float ratio_h = 352.0f / input.h;
	for (unsigned int h = 0; h < output.h; h++)
	{
		for (unsigned int w = 0; w < output.w; w++)
		{
			// Calculate score
			int obj_score_idx = 0 * output.h * output.w + h * output.w + w;
			int cls_score_idx = 5 * output.h * output.w + h * output.w + w;
			float obj_score = output.data[obj_score_idx];
			float cls_score = output.data[cls_score_idx];
			float score = pow(cls_score, 0.4) * pow(obj_score, 0.6);
			// Filter bbox
			if (score > output.threshold){
				// Resolve coordinates
                int x_offset_idx = 1 * output.h * output.w + h * output.w + w;
                int y_offset_idx = 2 * output.h * output.w + h * output.w + w;
                int box_width_idx = 3 * output.h * output.w + h * output.w + w;
                int box_height_idx = 4 * output.h * output.w + h * output.w + w;

                float x_offset = Tanh(output.data[x_offset_idx]);
                float y_offset = Tanh(output.data[y_offset_idx]);
                float box_width = Sigmoid(output.data[box_width_idx]) * ratio_w;
                float box_height = Sigmoid(output.data[box_height_idx]) * ratio_h;

                float cx = (w + x_offset) / output.w;
                float cy = (h + y_offset) / output.h;

                int x1 = UVITA_MAX(0, (int)((cx - box_width * 0.5) * input.w));
                int y1 = UVITA_MAX(0, (int)((cy - box_height * 0.5) * input.h));
                int x2 = UVITA_MIN(input.w, (int)((cx + box_width * 0.5) * input.w));
                int y2 = UVITA_MIN(input.h, (int)((cy + box_height * 0.5) * input.h));

                pboxes[cnt].rect[0] = x1;
                pboxes[cnt].rect[1] = y1;
                pboxes[cnt].rect[2] = x2;
                pboxes[cnt].rect[3] = y2;
                pboxes[cnt].score = score;
                cnt++;
			}
		}
	}
	return cnt;
}

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
                               uvita_person_out* pout)
{
  if (frame->height != DPERSON_MODEL_INPUT_H
      || frame->width != DPERSON_MODEL_INPUT_W
	  || frame->data == NULL) {
	  return UVITA_ARGUMENT_ERROR;
  }
  float* dperson_imageData = (float*)dperson_inputAddr;
  // Produce input data for bundle.
  // Copy the pre-processed image data into the bundle input buffer.
  trans_hwc2chw(frame->data, dperson_activations, DPERSON_MODEL_INPUT_H, DPERSON_MODEL_INPUT_W, DPERSON_MODEL_INPUT_C);

  // preprocess with given mean and scale vaule
  for (int i = 0; i < PERSON_INPUT_SIZE; i += 3) {
	  dperson_imageData[i] = ((float)(dperson_activations[i]) - DPERSON_MODEL_INPUT_MEAN) * DPERSON_MODEL_INPUT_SCALE;
	  dperson_imageData[i + 1] = ((float)(dperson_activations[i+1]) - DPERSON_MODEL_INPUT_MEAN) * DPERSON_MODEL_INPUT_SCALE;
	  dperson_imageData[i + 2] = ((float)(dperson_activations[i+2]) - DPERSON_MODEL_INPUT_MEAN) * DPERSON_MODEL_INPUT_SCALE;
  }

  dperson_shufflenetv2((uint8_t *)dperson_constantWeight, dperson_mutableWeight, dperson_activations);

  float *out_data = (float*)(dperson_outputAddr);

  // Model input initialize
  uvita_nn_input shufflenetv2_input;
  shufflenetv2_input.h = DPERSON_MODEL_INPUT_H;
  shufflenetv2_input.w = DPERSON_MODEL_INPUT_W;
  // Model output initialize
  uvita_nn_output shufflenetv2_output;
  shufflenetv2_output.c = DPERSON_MODEL_OUTPUT_C;
  shufflenetv2_output.h = DPERSON_MODEL_OUTPUT_H;
  shufflenetv2_output.w = DPERSON_MODEL_OUTPUT_W;
  shufflenetv2_output.data = out_data;
  shufflenetv2_output.threshold = DPERSON_THRESHOLD;

  DetBox_t* personboxes = (DetBox_t*)malloc(shufflenetv2_output.h*shufflenetv2_output.c*sizeof(DetBox_t));

  int box_num = anchor_free_process(shufflenetv2_input, shufflenetv2_output, personboxes);

  pout->has_person = uvita_nms(personboxes, box_num);

  for(int i=0; i<pout->has_person; i++) {
	  memcpy(&(pout->boxes[i]), &personboxes[i], sizeof(DetBox_t));
  }

  free((uint8_t*)personboxes);

  return UVITA_SUCCESS;
}

