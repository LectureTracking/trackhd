//
// Created by charles on 2016/07/20.
//

#include "IlluminationCorrection.h"

#include "opencv2/opencv.hpp"
#include <vector>

int useAlgorithm = 1;

using namespace cv;
using namespace std;

void IlluminationCorrection::histogramNormalisation(std::vector<Mat> &mFrames) {

    /*
    namedWindow("Light Correction - Original", WINDOW_NORMAL);
    resizeWindow("Light Correction - Original", 1080, 720);

    namedWindow("Light Correction - Normalised", WINDOW_NORMAL);
    resizeWindow("Light Correction - Normalised", 1080, 720);
*/
    ///CLAHE (Contrast Limited Adaptive Histogram Equalization)
    std::vector<cv::Mat> temp;
    for (int i = 0; i < mFrames.size(); i++) {

        switch (useAlgorithm) {
            case 1: {
                // READ RGB color image and convert it to Lab
                cv::Mat bgr_image = mFrames[i];
                cv::Mat lab_image;
                cv::cvtColor(bgr_image, lab_image, CV_BGR2Lab);


                // Extract the L channel
                std::vector<cv::Mat> lab_planes(3);
                cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]


                // apply the CLAHE algorithm to the L channel
                cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
                clahe->setClipLimit(1);
                cv::Mat dst;
                clahe->apply(lab_planes[0], dst);

                // Merge the the color planes back into an Lab image
                dst.copyTo(lab_planes[0]);
                cv::merge(lab_planes, lab_image);

                // convert back to RGB
                cv::Mat image_clahe;
                cv::cvtColor(lab_image, image_clahe, CV_Lab2BGR);


                //imshow("Light Correction - Original", mFrames.at(i).getColourFrame().clone());
                // imshow("Light Correction - Normalised", image_clahe);
                //waitKey(0);

                //Update video
                mFrames[i] = image_clahe;
                break;
            }
            case 2: {
                ///Histograms Equalization

                ///Algorithm 2
                cv::Mat src = mFrames[i];
                cvtColor(src, src, CV_BGR2GRAY);

                //Equalise
                equalizeHist(src, src);
                cvtColor(src, src, CV_GRAY2BGR);
                //imshow("Light Correction - Original", mFrames.at(i).getColourFrame().clone());
                //imshow("Light Correction - Normalised", src);
                //waitKey(0);
                mFrames[i]=src;
                break;
            }

            case 3: {

                cv::Mat src = mFrames[i];
                src.convertTo(src, -1, 1, 75);
                // imshow("Light Correction - Original", mFrames.at(i).getColourFrame().clone());
                //imshow("Light Correction - Normalised", src);
                //waitKey(0);
                break;
            }
            default:
                cout << "None used" << endl;

        }

    }
    // mFrames.clear();
    // mFrames = std::move(temp);

}


void IlluminationCorrection::histogramNormalisation(Mat &frame)
{

    ///CLAHE (Contrast Limited Adaptive Histogram Equalization)


    switch (useAlgorithm)
    {
        case 1:
        {
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


            //imshow("Light Correction - Original", mFrames.at(i).getColourFrame().clone());
            // imshow("Light Correction - Normalised", image_clahe);
            //waitKey(0);

            //Update video
            frame = image_clahe;
            break;
        }
        case 2:
        {
            ///Histograms Equalization

            ///Algorithm 2
            cv::Mat src = frame;
            cvtColor(src, src, CV_BGR2GRAY);

            //Equalise
            equalizeHist(src, src);
            cvtColor(src, src, CV_GRAY2BGR);
            //imshow("Light Correction - Original", mFrames.at(i).getColourFrame().clone());
            //imshow("Light Correction - Normalised", src);
            //waitKey(0);
            frame = src;
            break;
        }

        case 3:
        {

            cv::Mat src = frame;
            src.convertTo(src, -1, 1, 75);
            // imshow("Light Correction - Original", mFrames.at(i).getColourFrame().clone());
            //imshow("Light Correction - Normalised", src);
            //waitKey(0);
            break;
        }
        default:
            cout << "None used" << endl;

    }

}


