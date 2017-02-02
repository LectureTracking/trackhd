/**
 *  Copyright 2016 Charles Fitzhenry / Mohamed Tanweer Khatieb / Maximilian Hahn
 *  Licensed under the Educational Community License, Version 2.0
 *  (the "License"); you may not use this file except in compliance
 *  with the License. You may obtain a copy of the License at
 *
 *  http://www.osedu.org/licenses/ECL-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS"
 *  BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 *  or implied. See the License for the specific language governing
 *  permissions and limitations under the License.
 *
 */

//
// Created by Tanweer Khatieb on 2016/07/21.
//

#include "FrameCropper.h"

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
