//
// Created by charles on 2016/07/20.
//

#include "MotionDetection.h"

#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int thresholdVal = 35;
bool showWindows = false;

int MotionDetection::subtract(std::vector<cv::Mat> &frames, PersistentData &persistentData) {

    cv::Mat gray1;
    cv::Mat gray2;
    cv::Mat diffImg;
    cv::Mat threshImg;
    cv::Mat threshAccumulation;

    for (int i = 0; i < frames.size() - 1; i++) {
        if (i > frames.size() - 1) {
            break;
        }


        //Convert frame to grayscale
        cv::cvtColor(frames[i], gray1, COLOR_BGRA2GRAY);
        cv::cvtColor(frames[i + 1], gray2, COLOR_BGRA2GRAY);



        //Perform background subtraction
        cv::absdiff(gray1, gray2, diffImg);


        cv::threshold(diffImg, threshImg, thresholdVal, 255, THRESH_BINARY);
        cv::blur(threshImg, threshImg, cv::Size(10, 10));
        cv::threshold(threshImg, threshImg, thresholdVal, 255, THRESH_BINARY);

        if (i == 0) {
            threshAccumulation = threshImg.clone();
        }

        bitwise_or(threshAccumulation, threshImg, threshAccumulation);

    }


    cv::Mat kernel = Mat::ones(10, 10, CV_32F);
    cv::morphologyEx(threshAccumulation, threshAccumulation, MORPH_OPEN, kernel);


    cv::blur(threshAccumulation, threshAccumulation, cv::Size(10, 10));
    cv::threshold(threshAccumulation, threshAccumulation, thresholdVal, 255, THRESH_BINARY);


    persistentData.areasOfMotion.push_back(boundMotion(threshAccumulation));

    //destroy GUI windows
    destroyAllWindows();
    return 1;
}


Rect MotionDetection::boundMotion(Mat threshold_output) {


    RNG rng(12345);
    //Mat threshold_output = threshAccumulation;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    int thresh = 20;

    cv::blur(threshold_output, threshold_output, cv::Size(100, 100));

    /// Find contours
    findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());


    for (int i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
    }

    //Find larges enclosing rectangle
    int top_x = INT_MAX;
    int top_y = INT_MAX;
    int bottom_x = INT_MIN;
    int bottom_y = INT_MIN;

    for (Rect r : boundRect) {
        int r_top_x = r.tl().x;
        int r_top_y = r.tl().y;
        int r_bottom_x = r.br().x;
        int r_bottom_y = r.br().y;

        if (r_top_x < top_x) {
            top_x = r_top_x;
        }
        if (r_top_y < top_y) {
            top_y = r_top_y;
        }
        if (r_bottom_x > bottom_x) {
            bottom_x = r_bottom_x;
        }
        if (r_bottom_y > bottom_y) {
            bottom_y = r_bottom_y;
        }
    }

    Rect overallMotionCrop(Point(top_x, top_y), Point(bottom_x, bottom_y));

    /// Draw polygonal contour + bonding rects + circles
    Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        rectangle(drawing, overallMotionCrop.tl(), overallMotionCrop.br(), color, 8, 8, 0);


    return overallMotionCrop;
}

