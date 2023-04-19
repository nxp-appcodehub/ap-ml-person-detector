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
    parser = argparse.ArgumentParser(description='person detection image test',
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
    images_path = ["../data/Tests/33_Running_Running_33_142.jpg",
                   "../data/Tests/34_Baseball_Baseball_34_19.jpg",
                   "../data/Tests/11_Meeting_Meeting_11_332.jpg",
                   "../data/Tests/37_Soccer_Soccer_37_220.jpg"]
    detector = dperson(args.model)

    for i in range(0, len(images_path)):
        img = cv2.imread(images_path[i])
        bboxes = detector.detect(img, iw, ih, args.thresh)
        detector.show(img)
        save_path = images_path[i].split("Tests")[0] + "Results/" + images_path[i].split("Tests")[1]
        cv2.imshow(images_path[i], img)
    cv2.waitKey(0)

