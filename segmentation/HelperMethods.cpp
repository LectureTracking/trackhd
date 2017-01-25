//
// Created by charles on 2017/01/18.
//

#include "HelperMethods.h"

using namespace cv;

bool HelperMethods::RectA_inside_RectB(cv::Rect r1, cv::Rect r2) {

    //Create intersection rectangle
    Rect r3 = r1 & r2;
    bool status = false;

    if (r3.area() > 0) {
        if (r3.area() == r2.area()) {
            //cout << "r2 is inside r1" << endl;
            status = false;
        } else if (r3.area() == r1.area()) {
            // cout << "r1 is inside r2" << endl;
            status = true;
        } else {
            //  cout << "Overlapping Rectangles" << endl;
            status = false;
        }
    } else {
        //  cout << "Non-overlapping Rectangles" << endl;
        status = false;

    }

    return status;
}
