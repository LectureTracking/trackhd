//
// Created by Tanweer Khatieb on 2016/07/21.
//

#ifndef TRACK4K_FRAMECROPPER_H
#define TRACK4K_FRAMECROPPER_H


#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <highgui.h>
#include "../MetaFrame.h"

class FrameCropper
{
public:
    cv::Mat crop(cv::Mat &frame, cv::Rect CroppingRect); //Add parameter for zoom functionality
    void cropFrames(std::vector<MetaFrame> &mF,  std::vector<cv::Mat> &output, std::vector<cv::Rect> &cropRectangles);
    void cropFrames(std::vector<cv::Mat> &mF, std::vector<cv::Mat> &output,
                                  std::vector<cv::Rect> &cropRectangles);


};


#endif //TRACK4K_FRAMECROPPER_H
