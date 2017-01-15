//
// Created by charles on 2016/07/19.
//

#include "../../CodeTimer.h"
#include "../headers/IlluminationCorrection.h"

#include "../headers/MotionDetection.h"
#include "../headers/Track4KPreProcess.h"
#include "opencv2/opencv.hpp"
#include "../../Panning/headers/VideoOutput.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

void Track4KPreProcess::preProcessDriver(PersistentData &pD)
{

    //Create a timer object to measure method execution times
    CodeTimer timer;
    CodeTimer timer2;
    vector<Mat> frameMats;


    //read in video file
    FileReader fileReader;
    fileReader.readFile(pD.inputFileName, pD);

    IlluminationCorrection i;
    MotionDetection m;
    BoardDetection bD;

    float fileDetectionTime = 0;
    float motionDetectionTime = 0;
    float boardDetectionTimer = 0;


    while (!fileReader.isEndOfFile())
    {

        timer.start();
        fileReader.getNextSegment(pD.areasOfMotionOverNumberOfFrames, frameMats);
        fileDetectionTime += timer.stop(1);

        timer.start();
        //m.subtract(frameMats, pD);
        motionDetectionTime += timer.stop(1);

        timer.start();
        bD.extractBoards(frameMats, pD);
        boardDetectionTimer += timer.stop(1);

    }


    fileReader.getInputVideo().release();

/*
    ofstream t;
    t.open("time.txt");


    t << "File read time: " << fileDetectionTime << endl;
    t << "Motion detection time: " << motionDetectionTime << endl;
    t << "Board detection time: " << boardDetectionTimer << endl;

    cout << "File read time: " << fileDetectionTime << endl;
    cout << "Motion detection time: " << motionDetectionTime << endl;
    cout << "Board detection time: " << boardDetectionTimer << endl;

    t.close();
*/
    //--------------------//
    // Write output video //
    //--------------------//

}
