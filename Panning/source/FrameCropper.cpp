//
// Created by Tanweer Khatieb on 2016/07/21.
//

#include "../headers/FrameCropper.h"

using namespace std;
using namespace cv;

cv::Mat FrameCropper::crop(cv::Mat &frame, cv::Rect newFrame) {
    //cv::imwrite("noises_cropped.png", frame(newFrame));
    return frame(newFrame);
}

void FrameCropper::cropFrames(std::vector<MetaFrame> &mF, std::vector<cv::Mat> &output,
                              std::vector<cv::Rect> &cropRectangles) {

    for (int i = 0; i < mF.size(); i++) {
        Mat t = mF.at(i).getColourFrame();
        Mat c = crop(t, cropRectangles.at(i));
        //fd.at(i).setColourFrame(c);
        output.push_back(c);
        //imshow("Frame",c);
        //waitKey(0);
    }
}

void FrameCropper::cropFrames(std::vector<Mat> &mF, std::vector<cv::Mat> &output,
                              std::vector<cv::Rect> &cropRectangles) {

    for (int i = 0; i < mF.size(); i++) {
        Mat t = mF.at(i);
        Mat c = crop(t, cropRectangles.at(i));
        //fd.at(i).setColourFrame(c);
        output.push_back(c);
        //imshow("Frame",c);
        //waitKey(0);
    }
}
