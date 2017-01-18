//
// Created by Charles Fitzhenry on 2016/07/19.
//

#include "../../CodeTimer.h"
#include "../headers/IlluminationCorrection.h"

#include "../headers/MotionDetection.h"
#include "../headers/Track4KPreProcess.h"

using namespace std;
using namespace cv;

void Track4KPreProcess::preProcessDriver(PersistentData &persistentData) {

    vector<Mat> frameMats;

    //Read in video file
    FileReader fileReader;
    fileReader.readFile(persistentData.inputFileName, persistentData);

    //Create objects
    MotionDetection motionDetection; //Detects and segments overall merged motion over a given number of frames
    BoardDetection boardDetection; //Detects boards

    //Keep reading in frames from the video file until the end is reached.
    //Number of frames to read on each iteration is defined in the PersistentData class
    while (!fileReader.isEndOfFile()) {
        //Read in frames
        fileReader.getNextSegment(persistentData.areasOfMotionOverNumberOfFrames, frameMats);

        //Detect areas of motion
        motionDetection.subtract(frameMats, persistentData);

        //Detect the boards
        boardDetection.extractBoards(frameMats, persistentData);

    }

    fileReader.getInputVideo().release();
}
