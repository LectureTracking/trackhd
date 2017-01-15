//
// Created by Fudge on 2016/09/05.
//

#ifndef TRACK4K_MOVEMENTDETECTION_H
#define TRACK4K_MOVEMENTDETECTION_H

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/video.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include "Ghost.h"

using namespace cv;
using namespace std;

class MovementDetection{

public:

    //method to check bounding box intersection (replaced by | and & overloaded params on Rect)
    bool BoundingBoxIntersect(Rect a, Rect b){
        return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
                (abs(a.y - b.y) * 2 < (a.height + b.height));
    }

    //return frameskipreset value
    int getFrameSkipReset(){ return frameSkipReset;}

    //get video name
    string getVideoDir(){ return videoDir; }

    //set show label
    void setShowLabel(string label){ showLabel = label; }

    //set video name
    void setVideoDir(string name){ videoDir = name; }

    //default constructor and tracking runner
    MovementDetection(string name, vector<Rect>* lect);

    //calculates the distance between the closest two edge points of two rectangles
    double closestDistance(Rect a, Rect b);

    //helper function to calculate distance between two points
    double euclideanDist(Point p, Point q);

    //check for overlapping rectangles and push them together
    //return true if changes made
    bool overlapCheck(vector<Rect>* boundingRects);

    //check for nearby rectangles and push them together
    //return true if changes made
    bool proximityCheck(vector<Rect>* boundingRects);

    //performs overlap and proximity checks until no more changes occur
    void overlapProximityLoop(vector<Rect>* boundingRects);

    //check if any massive rectangles exist in the scene
    bool oversizeCheck(vector<Rect>* boundingRects, int width, int height);

    //add text to frame function
    void addTextToFrame(Mat* frame, string str, auto value);

    //values to reset on loop
    void onLoopReset();

    //push overalpping ghosts together
    void ghostOverlapCheck(vector<Ghost>* ghostRects);

    //show frames using ghostRectangles
    void showFramesMemoryRects(vector<vector<Ghost>>* memoryGhosts, Scalar color);

    //reposition ghost to centroid of squares
    void adjustLecturer(vector<Rect>* lecturer, vector<vector<Rect>>* memRects, vector<Rect>* newLect);

    //add new ghosts (rectangles that don't intersect anywhere
    void addNewGhosts(vector<Ghost>* ghostRects, vector<Rect>* boundingRects);

    //return timeOnScreen * (1 / (ratioOfDistFromCenter ^ 2)) averaged between x and y
    double positionWeighting(Rect r, long onScreenTime);

    //divide ghosts with multiple rects in it that are far apart
    void ghostSplitCheck(vector<Ghost>* ghostRects, vector<Rect>* boundingRect);

    //find lecturer based on onScreenTime and position on the screen
    void findLecturer(vector<vector<Ghost>>* memoryGhosts, vector<Rect>* lecturer);

    //convert frame number to string && hours:minutes:seconds
    string convertFrameToTime(long frameNumber, double fps);

    //copy lecturer values across
    void getLecturer(vector<Rect>* newLect);

    //write video
    void writeVideo(vector<Rect>* lecturer, string outName);

private:

    //frames used during processing
    Mat frame; //current frame
    Mat prevFrame; //previous frame for diff
    Mat frameAbsDiff; //absolute difference (change) frame
    Mat frameThresh; //thresholding
    Mat frameMorph; //morphological operations
    Mat frameBlur; //blur operation
    Mat frameDisplay; //the frame that we print to imShow()

    //constant values used to process
    int aspectR = 3; //aspect ratio threshold of square (width / height) to delete
    int frameSkipReset = 3; //number of frames to skip per interval
    double clusterThreshold = 50; //distance between objects before they are considered separate
    double massiveThreshold = 0.3; //value from 0.1 to 1 that represents how much area a square can be of the screen before its too large
    string frameText = "";
    stringstream ss;
    int textCount = 0;
    int contourMinSize = 20; //min threshold value for array size of contour
    double lowScreenThresh = 0.3; //lowest point that rect tl().y is valid (0 - 1)
    double highScreenThresh = 0.3; //highest point that rect br().y is valid (0 - 1)
    int ghostResetValue = 100; //number of frames to do before resetting counts
    int ghostResetAmount = (int)(ghostResetValue * 0.67); //number to reduce every {ghostResetValue} frames
    string videoDir = "";
    string showLabel = "MyVideo";
    bool validFps = false;
    double shiftR = 0.7; //value between 0.1 and 0.9 symbolizing how much a single rect will shift the ghost
    double shiftRInv = 1 - shiftR; //inverse of ghostShiftRatio
    Point center; //center of the screen
    int minSplitDist = 150; //minimum distance to bother splitting ghosts

    //debugging

    //draw visuals
    bool drawFrameStatus = true;
    /*
     * 0 - draw contours
     * 1 - draw bounding rectangles
     * 2 - draw ghosts
     * 3 - draw ghost time on screen
     * 4 - write time
     * 5 - write frame number
     * 6 - write number of rectangles
     */
    bool drawSettings[7] = {true, true, true, true, true, true, true};

    //write text to console
    bool showFrameStatus = true;

    //write video file information
    bool showFileInfo = true;

    //vectors that are saved for final processing step
    vector<vector<Rect>> memoryRects; //list of all rectangles
    vector<vector<Ghost>> memoryGhosts; //list of all ghosts
    vector<Rect> lecturer; //positions on lecturer
};




#endif //TRACK4K_MOVEMENTDETECTION_H
