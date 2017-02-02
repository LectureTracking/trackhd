//
// Created by Charles on 2016/10/25.
//


#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

#include "PointPlotter.h"

bool toggle = true;
int s = 0;

void onMouse3(int evt, int x, int y, int flags, void *param)
{
    /*if (toggle && s > 0)
    {
        std::vector<int> *ptPtr = (std::vector<int> *) param;
        ptPtr->push_back(x);
        cout << "Mouse Click: x=" << x << " y=" << y << endl;
        toggle = !toggle;
    } else if (evt == EVENT_LBUTTONDOWN & toggle && s == 0)
    {
        std::vector<int> *ptPtr = (std::vector<int> *) param;
        ptPtr->push_back(x);
        cout << "Mouse Click: x=" << x << " y=" << y << endl;
        toggle = !toggle;
    }*/

    if (evt == EVENT_LBUTTONDOWN){
        std::vector<int> *ptPtr = (std::vector<int> *) param;
        ptPtr->push_back(x);
        cout << "Mouse Click: x=" << x << " y=" << y << endl;
    }


    /*if (flags == EVENT_FLAG_LBUTTON && toggle)
    {
        cout << "Mouse held down" << endl;
        std::vector<int> *ptPtr = (std::vector<int> *) param;
        ptPtr->push_back(x);
        cout << "Mouse Click: x=" << x << " y=" << y << endl;
        toggle = !toggle;
    }*/
}

std::vector<int> points2;

void PointPlotter::plotPoints(PersistentData &pD)
{
    vector<Mat> frames;

    FileReader fileReader;
    fileReader.readFile("Videos/presenter-MAM1000W-20160811.mp4", pD);

    fileReader.getNextSegment(1160, frames);

    //For debugging
    namedWindow("Frame", WINDOW_NORMAL);
    resizeWindow("Frame", 1080, 720);

    for (int i = 0; i < frames.size(); i+=29)
    {
        s = i;
        imshow("Frame", frames[i]);
        setMouseCallback("Frame", onMouse3, (void *) &points2);
        /*if (i == 0)
        {
            waitKey(0);
            toggle = !toggle;
        } else
        {
            waitKey(200);
            toggle = !toggle;
        }*/
        waitKey(0);
    }


    if (points2.size() > 2) //we have 2 points
    {
        ofstream myfile;
        myfile.open("newPoints29.txt");

        for (auto it = points2.begin(); it != points2.end(); ++it)
        {
            cout << "X and Y coordinates are given below" << endl;
            cout << (*it) << '\t' << 120 << endl;
            myfile << (*it) << " " << 800 << "\n";
        }
        //draw points
        myfile.close();
    }
}
