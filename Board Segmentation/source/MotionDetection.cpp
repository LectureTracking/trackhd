//
// Created by charles on 2016/07/20.
//

#include "../headers/MotionDetection.h"

#include <iostream>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int thresholdVal = 35;
bool showWindows = false;
int MotionDetection::subtract(std::vector<cv::Mat> &frames, PersistentData &pD) {

    //create Background Subtractor objects
    pMOG2 = createBackgroundSubtractorMOG2(1, 60); //MOG2 approach


    //create GUI windows

    if (showWindows) {
        namedWindow("Threshold", WINDOW_NORMAL);
        resizeWindow("Threshold", 1080, 720);

        namedWindow("threshImg", WINDOW_NORMAL);
        resizeWindow("threshImg", 1080, 720);

        namedWindow("Contours", WINDOW_NORMAL);
        resizeWindow("Contours", 1080, 720);
    }


    namedWindow("Contours", WINDOW_NORMAL);
    resizeWindow("Contours", 1080, 720);


    cv::Mat gray1;
    cv::Mat gray2;
    cv::Mat diffImg;
    cv::Mat threshImg;
    cv::Mat threshAccumulation;

    for (int i = 0; i < frames.size() - 1; i++) {
        if(i>frames.size()-1){
            break;
        }


        //Convert frame to grayscale
        cv::cvtColor(frames[i], gray1, COLOR_BGRA2GRAY);
        cv::cvtColor(frames[i+1], gray2, COLOR_BGRA2GRAY);



        //Perform background subtraction
        cv::absdiff(gray1, gray2, diffImg);




        cv::threshold(diffImg, threshImg, thresholdVal, 255, THRESH_BINARY);
        cv::blur(threshImg, threshImg, cv::Size(10, 10));
        cv::threshold(threshImg, threshImg, thresholdVal, 255, THRESH_BINARY);

        if (i == 0) {
            threshAccumulation = threshImg.clone();
        }

        //imshow("Contours",frames.at(i).getColourFrame());
        //waitKey(0);




        //boundMotion(threshImg);

        bitwise_or(threshAccumulation, threshImg, threshAccumulation);




        /*imshow("Frame", diffImg);
        imshow("FG Mask MOG 2", threshImg);
        imshow("Threshold", threshAccumulation);
        waitKey(0);*/

        // processVideo(frames.at(i));


    }


    if (showWindows) {
        cout << "Showing Threshold 1" << endl;
        imshow("threshImg", threshAccumulation);
        waitKey(0);
    }

    cv::Mat kernel = Mat::ones(10, 10, CV_32F);
    cv::morphologyEx(threshAccumulation, threshAccumulation, MORPH_OPEN, kernel);

    if (showWindows) {
        cout << "Showing Threshold 2" << endl;
        imshow("threshImg", threshAccumulation);
        waitKey(0);
    }

    cv::blur(threshAccumulation, threshAccumulation, cv::Size(10, 10));
    cv::threshold(threshAccumulation, threshAccumulation, thresholdVal, 255, THRESH_BINARY);

    if (showWindows) {
        cout << "Showing Threshold 3" << endl;
        imshow("Threshold", threshAccumulation);
        imwrite("threshAccumulation.png", threshAccumulation);
        waitKey(0);
    }

    pD.areasOfMotion.push_back(boundMotion(threshAccumulation));

    //destroy GUI windows
    destroyAllWindows();
    return 1;
}


vector<Rect> MotionDetection::boundMotion(Mat threshold_output) {


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


    vector<int> toRemove;
    //Remove rectangles contained in another

    for (int i = 0; i < contours.size(); i++) {

        Rect r1 = boundRect[i];
        for (int j = i + 1; j < contours.size(); j++) {

           // cout << "Comparing rect " << i << " with " << j << endl;

            Rect r2 = boundRect[j];
            Rect r3 = r1 & r2;

            if (r3.area() > 0) {
                if (r3.area() == r2.area()) {
                   //cout << "r2 is inside r1" << endl;
                    toRemove.push_back(j);
                } else if (r3.area() == r1.area()) {
                   // cout << "r1 is inside r2" << endl;
                    toRemove.push_back(i);
                } else {
                  //  cout << "Overlapping Rectangles" << endl;
                }
            } else {
              //  cout << "Non-overlapping Rectangles" << endl;
            }
        }
    }

    //Remove jth rect
    vector<Rect> nR;

    for (int i = 0; i < boundRect.size(); i++) {
        if (std::find(toRemove.begin(), toRemove.end(), i) != toRemove.end()) {

        } else {
            nR.push_back(boundRect.at(i));
        }
    }


    if (showWindows || 1) {
    /// Draw polygonal contour + bonding rects + circles
    Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
    for (int i = 0; i < nR.size(); i++) {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        //rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
        rectangle(drawing, nR[i].tl(), nR[i].br(), color, 8, 8, 0); }


        /// Show in a window
        cout << "Contours" << endl;
        imshow("Contours", drawing);
        imwrite("Bounded.png", drawing);
        waitKey(0);
    }

    return nR;
}

void MotionDetection::processVideo(MetaFrame &frame) {
    cout << "Subtraction" << endl;
    cv::Mat kernel = Mat::ones(3, 3, CV_32F);
    //update the background model
    pMOG2->apply(frame.getColourFrame(), fgMaskMOG2);

    //show the current frame and the fg masks
    imshow("Frame", frame.getColourFrame());
    //cv::blur(fgMaskMOG2,fgMaskMOG2,cv::Size(5,5));
    //cv::morphologyEx(fgMaskMOG2,fgMaskMOG2,MORPH_ERODE,kernel);
    imshow("FG Mask MOG 2", fgMaskMOG2);
    waitKey(0);


    //get the input from the keyboard

}