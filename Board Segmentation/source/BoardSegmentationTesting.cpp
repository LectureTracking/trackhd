//
// Created by Charles on 2016/11/05.
//

#include "../headers/BoardSegmentationTesting.h"
#include "../headers/IlluminationCorrection.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;
void BoardSegmentationTesting::colourCorrectionTest(std::string s){
    IlluminationCorrection iC;

    cout << "1" << endl;
    Mat I = imread(s);
    cout << "2" << endl;
   iC.histogramNormalisation(I);
    cout << "3" << endl;
    //Save frame again
    imwrite("CLAHE33.png", I);
    cout << "4" << endl;
}