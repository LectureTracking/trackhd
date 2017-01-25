//
// Created by charles on 2016/07/20.
//

#ifndef TRACK4K_ILLUMINATIONCORRECTION_H
#define TRACK4K_ILLUMINATIONCORRECTION_H

#include <opencv2/core/core.hpp>
#include <vector>
#include "../MetaFrame.h"

class IlluminationCorrection {
public:
    void histogramNormalisation(std::vector<cv::Mat> &mFrames);

    void histogramNormalisation(cv::Mat &frame);
};


#endif //TRACK4K_ILLUMINATIONCORRECTION_H
