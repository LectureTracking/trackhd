//
// Created by Charles on 2016/07/15.
//

#include "PersistentData.h"


void PersistentData::setVideoInfo(int f, int t, cv::Size s)
{
    if(!videoInfoSet){
        fps = f;
        totalFrames = t;
        videoDimension = s;
        videoInfoSet = true;
    }

}
