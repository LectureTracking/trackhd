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

#include "MotionDetection.h"

#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int thresholdVal = 35;

void MotionDetection::subtract(std::vector<cv::Mat> &frames, PersistentData &persistentData)
{

    //Two adjacent frames that would store grayscale images
    cv::Mat grayFrame1;
    cv::Mat grayFrame2;

    //Difference between the two frames
    cv::Mat differenceImage;
    cv::Mat thresholdImage;

    //A frame containing the accumulates superimposed difference frames
    cv::Mat thresholdAccumulation;

    // Loop over all frames and perform background subtraction
    for (int i = 0; i < frames.size() - 1; i++)
    {
        if (i > frames.size() - 1)
        {
            break;
        }

        //Convert frame to grayscale
        cv::cvtColor(frames[i], grayFrame1, COLOR_BGRA2GRAY);
        cv::cvtColor(frames[i + 1], grayFrame2, COLOR_BGRA2GRAY);


        //Perform background subtraction
        cv::absdiff(grayFrame1, grayFrame2, differenceImage);


        cv::threshold(differenceImage, thresholdImage, thresholdVal, 255, THRESH_BINARY);
        cv::blur(thresholdImage, thresholdImage, cv::Size(10, 10));
        cv::threshold(thresholdImage, thresholdImage, thresholdVal, 255, THRESH_BINARY);

        if (i == 0)
        {
            thresholdAccumulation = thresholdImage.clone();
        }

        bitwise_or(thresholdAccumulation, thresholdImage, thresholdAccumulation);

    }


    cv::Mat kernel = Mat::ones(10, 10, CV_32F);
    cv::morphologyEx(thresholdAccumulation, thresholdAccumulation, MORPH_OPEN, kernel);


    cv::blur(thresholdAccumulation, thresholdAccumulation, cv::Size(10, 10));
    cv::threshold(thresholdAccumulation, thresholdAccumulation, thresholdVal, 255, THRESH_BINARY);


    persistentData.areasOfMotion.push_back(boundMotion(thresholdAccumulation));

}

//Bound motion with rectangles
Rect MotionDetection::boundMotion(Mat threshold_output)
{

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    cv::blur(threshold_output, threshold_output, cv::Size(100, 100));

    // Find contours
    findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    // Approximate contours to polygons + get bounding rectangles
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());

    //Bound all motion with rectangles
    for (int i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
    }

    //Find larges enclosing rectangle from all small rectangles
    int top_x = INT_MAX;
    int top_y = INT_MAX;
    int bottom_x = INT_MIN;
    int bottom_y = INT_MIN;

    for (Rect r : boundRect)
    {
        int r_top_x = r.tl().x;
        int r_top_y = r.tl().y;
        int r_bottom_x = r.br().x;
        int r_bottom_y = r.br().y;

        if (r_top_x < top_x)
        {
            top_x = r_top_x;
        }
        if (r_top_y < top_y)
        {
            top_y = r_top_y;
        }
        if (r_bottom_x > bottom_x)
        {
            bottom_x = r_bottom_x;
        }
        if (r_bottom_y > bottom_y)
        {
            bottom_y = r_bottom_y;
        }
    }

    Rect overallMotionCrop(Point(top_x, top_y), Point(bottom_x, bottom_y));

    return overallMotionCrop;
}