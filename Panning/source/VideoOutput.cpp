//
// Created by Tanweer Khatieb on 2016/07/21.
//

#include "../headers/VideoOutput.h"

using namespace std;
using namespace cv;

void VideoOutput::write(std::string fileName, std::vector<MetaFrame> &frames, cv::Size dimensions, int extension, int fps, bool changeCodec)
{
    VideoWriter outputVideo;
    outputVideo.open(fileName, extension, fps, dimensions, changeCodec);

    if (outputVideo.isOpened())
    {
        cout << "Ready to write" << endl;
        for (MetaFrame f:frames)
        {
            outputVideo.write(f.getColourFrame());
        }
    }
}

void VideoOutput::write(std::string fileName, std::vector<cv::Mat> &frames, cv::Size dimensions, int extension, int fps, bool changeCodec){
    VideoWriter outputVideo;
    outputVideo.open(fileName, extension, fps, dimensions, changeCodec);

    if (outputVideo.isOpened())
    {
        cout << "Ready to write" << endl;
        for (Mat f:frames)
        {
            outputVideo.write(f);
        }
    }
}

