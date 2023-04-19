#
# Copyright 2022 NXP
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
import cv2
import numpy as np
import onnxruntime 

# sigmoid function
def sigmoid(x):
    return 1. / (1 + np.exp(-x))
# tanh function
def tanh(x):
    return 2. / (1 + np.exp(-2 * x)) - 1

class dperson:
    def __init__(self, onnxmModelPath):
        self.session = onnxruntime.InferenceSession(onnxmModelPath)
        self.bbox = None

    def preprocess(self, img, size):
        output = cv2.resize(img, (size[0], size[1]), interpolation=cv2.INTER_AREA)
        output = output.transpose(2, 0, 1)
        output = output.reshape((1, 3, size[1], size[0])) / 255
        return output.astype('float32')

    def nms(self, bbox, thresh=0.4):
        if len(bbox) > 0:
            # bbox: N*M, where N is the number of the bounding boxes, (x1, y1, x2, y2, score)
            x1 = bbox[:, 0]
            y1 = bbox[:, 1]
            x2 = bbox[:, 2]
            y2 = bbox[:, 3]
            scores = bbox[:, 4]
            areas = (x2 - x1 + 1) * (y2 - y1 + 1)  # the area of each box
            order = scores.argsort()[::-1]  # sort the bounding box according to score
            keep = []  # save the bounding boxes
            while order.size > 0:
                i = order[0]  # save the bbox of highest score without limitation
                keep.append(i)
                # compute the cross area between the current bbox and the others
                xx1 = np.maximum(x1[i], x1[order[1:]])
                yy1 = np.maximum(y1[i], y1[order[1:]])
                xx2 = np.minimum(x2[i], x2[order[1:]])
                yy2 = np.minimum(y2[i], y2[order[1:]])
                w = np.maximum(0.0, xx2 - xx1 + 1)
                h = np.maximum(0.0, yy2 - yy1 + 1)
                inter = w * h
                # compute the percentage cross area
                ovr = inter / (areas[i] + areas[order[1:]] - inter)
                # save the one whose cross area is lower than threshold
                order = order[np.where(ovr <= thresh)[0] + 1]
            # output = []
            # for i in keep:
            #     output.append(bbox[i].tolist())
            return bbox[keep].tolist()
        else:
            return bbox

    def detect(self, img, iw, ih, thresh):
        img_h, img_w, img_c = img.shape
        ratio_w, ratio_h = 352 / iw, 352 / ih
        data = self.preprocess(img, [iw, ih])
        # model inference
        input_name = self.session.get_inputs()[0].name
        feature_map = self.session.run([], {input_name: data})[0][0]
        # feature map converter CHW2HWC
        feature_map = feature_map.transpose(1, 2, 0)
        # get the height and width of the feature map
        fmh = feature_map.shape[0]
        fmw = feature_map.shape[1]
        self.bbox = []
        # post-process for feature map
        for h in range(fmh):
            for w in range(fmw):
                data = feature_map[h][w]
                # the confidence of the bounding boxes
                obj_score, cls_score = data[0], data[5:].max()
                score = (obj_score ** 0.6) * (cls_score ** 0.4)
                if score > thresh:
                    # the category of the class
                    cls_index = np.argmax(data[5:])
                    # the offset of the center location
                    x_offset, y_offset = tanh(data[1]), tanh(data[2])
                    # the normalization of the width and height
                    box_width, box_height = sigmoid(data[3])*ratio_w, sigmoid(data[4])*ratio_h
                    # the center of the box
                    box_cx = (w + x_offset) / fmw
                    box_cy = (h + y_offset) / fmh
                    # cx,cy,w,h => x1, y1, x2, y2
                    x1, y1 = box_cx - 0.5 * box_width, box_cy - 0.5 * box_height
                    x2, y2 = box_cx + 0.5 * box_width, box_cy + 0.5 * box_height
                    x1, y1, x2, y2 = int(x1 * img_w), int(y1 * img_h), int(x2 * img_w), int(y2 * img_h)

                    self.bbox.append([x1, y1, x2, y2, score, cls_index])
        self.bbox = self.nms(np.array(self.bbox))
        return self.bbox

    def show(self, img):
        font = cv2.FONT_HERSHEY_SIMPLEX # font
        fontScale = 3/4 # fontScale
        color = (255, 0, 0) # purple color in BGR
        thickness = 2 # Line thickness of 2 px
        h, w, c = img.shape
        for box in self.bbox:
            obj_score, cls_index = box[4], int(box[5])
            x1, y1, x2, y2 = int(box[0]), int(box[1]), int(box[2]), int(box[3])
            x1, y1, x2, y2 = max(0, x1), max(0, y1), min(w, x2), min(h, y2)
            cv2.rectangle(img, (x1, y1), (x2, y2), color, thickness)
            org = (x1, y1)  # org
            cv2.putText(img, 'person: ' + str(round(obj_score, 3)), org, font,
                        fontScale, (0, 255, 255), thickness, cv2.LINE_AA)
        return img