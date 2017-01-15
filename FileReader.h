//
// Created by Charles on 2016/07/15.
//

#ifndef TRACK4K_PREPROCESS_H
#define TRACK4K_PREPROCESS_H

#include "MetaFrame.h"
#include "opencv2/opencv.hpp"
#include "PersistentData.h"

class FileReader
{
private:
    cv::VideoCapture inputVideo;
    cv::Mat frame; //current frame
    int fps; //Frame Rate
    int numFrames; //Number of frames
    int videoDuration;
    cv::Size videoDimension;
    int ex;
    bool endOfFile = false;

public:
    bool isEndOfFile();

    bool readFile(std::string filename, PersistentData &pD);

    void getNextSegment(int segSize, std::vector<MetaFrame> &frameVec);

    void getNextSegment(int segSize, std::vector<cv::Mat> &frameVec);

    void getNextFrame(cv::Mat &frame);

    cv::VideoCapture &getInputVideo()
    {
        return inputVideo;
    }

    int getFps() const
    {
        return fps;
    }

    int getNumFrames() const
    {
        return numFrames;
    }

    int getVideoDuration() const
    {
        return videoDuration;
    }

    cv::Mat &getFrame()
    {
        return frame;
    }

    const cv::Size &getVideoDimension() const
    {
        return videoDimension;
    }

    int getEx() const
    {
        return ex;
    }
};

#endif //TRACK4K_PREPROCESS_H
