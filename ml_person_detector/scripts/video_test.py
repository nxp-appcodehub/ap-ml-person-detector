#
# Copyright 2022 NXP
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
import cv2
import argparse
from dperson import dperson

def parse_args():
    parser = argparse.ArgumentParser(description='person detection test',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--model', dest='model', required=False, type=str,
                        default='../models/Onnx/dperson_shufflenetv2.onnx')

    parser.add_argument('--thresh', dest='thresh', required=False, type=float,
                        default=0.85)

    args = parser.parse_args()
    return args

if __name__ == '__main__':
    args = parse_args()
    # input width and input height
    iw, ih = 320, 192
    detector = dperson(args.model)

    cap = cv2.VideoCapture(0)
    while True:
        ret, img = cap.read()
        bboxes = detector.detect(img, iw, ih, args.thresh)
        img = detector.show(img)
        cv2.imshow("person detector", img)
        if cv2.waitKey(1) & 0xFF == 27:
            break
