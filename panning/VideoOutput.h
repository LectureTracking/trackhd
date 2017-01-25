//
// Created by Tanweer Khatieb on 2016/07/21.
//

#ifndef TRACK4K_VIDEOOUTPUT_H
#define TRACK4K_VIDEOOUTPUT_H

#include "VideoOutput.h"
#include <opencv2/core/mat.hpp>
#include "opencv2/opencv.hpp"
#include "../MetaFrame.h"

class VideoOutput
{
public:
    void write(std::string fileName, std::vector<MetaFrame> &frames, cv::Size dimensions, int extension, int fps, bool changeCodec);
    void write(std::string fileName, std::vector<cv::Mat> &frames, cv::Size dimensions, int extension, int fps, bool changeCodec);

};


#endif //TRACK4K_VIDEOOUTPUT_H
