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
// Created by Charles Fitzhenry on 2016/07/20.
//

#include "IlluminationCorrection.h"

#include "opencv2/opencv.hpp"
#include <vector>

using namespace cv;
using namespace std;

void IlluminationCorrection::correctLight(std::vector<Mat> &mFrames)
{

    ///CLAHE (Contrast Limited Adaptive Histogram Equalization)
    std::vector<cv::Mat> temp;
    for (int i = 0; i < mFrames.size(); i++)
    {
        //Apply CLAHE algorithm to each frame
        applyCLAHE(mFrames[i]);
    }
}

//Method to apply light correction on a single frame
void IlluminationCorrection::applyCLAHE(Mat &frame)
{

    ///CLAHE (Contrast Limited Adaptive Histogram Equalization)
    // READ RGB color image and convert it to Lab
    cv::Mat bgr_image = frame;
    cv::Mat lab_image;
    cv::cvtColor(bgr_image, lab_image, CV_BGR2Lab);

    // Extract the L channel
    std::vector<cv::Mat> lab_planes(3);
    cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

    // apply the CLAHE algorithm to the L channel
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(3);
    cv::Mat dst;
    clahe->apply(lab_planes[0], dst);

    // Merge the the color planes back into an Lab image
    dst.copyTo(lab_planes[0]);
    cv::merge(lab_planes, lab_image);

    // convert back to RGB
    cv::Mat image_clahe;
    cv::cvtColor(lab_image, image_clahe, CV_Lab2BGR);

    //Update video
    frame = image_clahe;
}


