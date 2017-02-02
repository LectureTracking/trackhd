//
// Created by Fudge on 2016/10/14.
//



#ifndef TRACK4K_GHOST_H
#define TRACK4K_GHOST_H

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

using namespace cv;
using namespace std;

class Ghost{
private:
    Rect ghost; //rectangle of the ghost
    long onScreenTime; //time existing on screen (number of frames)
    double sizeRatio = 0.75; //amount that a resize affects the ghost
    double shrinkRatio = 0.1; //the amount by which a rect shrinks each turn
    double shrinkThresh = 0.6; //thresh value (0.1 - 1) of how full rect needs to be to avoid shrinking
    int deleteThresh = 40;

public:
    //ghost constructor
    Ghost(Rect g);
    //ghost constructor including time
    Ghost(Rect g, long screenTime);
    //return on screen time
    long getOnScreenTime(){ return onScreenTime; }
    //set on screen time
    void setOnScreenTime(long time){ onScreenTime = time; }
    //set on screen time
    void subOnScreenTime(long time){
        onScreenTime -= time;

        if(onScreenTime < 0){
            onScreenTime = 1;
        }
    }
    //return Rect dimensions
    Rect getGhost(){ return ghost; }
    //reset the variable ghost;
    void reset(Rect newGhost);
    //change the dimensions of ghost with intersecting rectangles
    bool resize(vector<Rect>* sizingRects);
    //decreases the size of the ghost, returns false to prompt delete
    bool decreaseArea();
    //get tl of ghost
    Point tl(){ return ghost.tl(); }
    //get br of ghost
    Point br(){ return ghost.br(); }
};

#endif //TRACK4K_GHOST_H
