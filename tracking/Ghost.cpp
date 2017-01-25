//
// Created by Fudge on 2016/10/14.
//

#include <iostream>
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

Ghost::Ghost(Rect g){
    ghost = g;
    onScreenTime = 1;
}

//other ghost constructor
Ghost::Ghost(Rect g, long screenTime){
    ghost = g;
    onScreenTime = screenTime;
}

//reset the variable ghost;
void Ghost::reset(Rect newGhost){
    ghost = newGhost;
}
//change the dimensions of the ghost with intersecting rectangles
//return true if the ghost needs to be deleted
bool Ghost::resize(vector<Rect>* sizingRects){
    int amountIntersected = 0;
    bool intersected = false; //if a rectangle intersects this ghost
    for (int i = 0; i < sizingRects->size(); i++) {

        //the intersection of sizingRects[i] and ghost
        Rect intersect;
        if(sizingRects->at(i).area() > ghost.area()){
            (intersect = sizingRects->at(i) & ghost);
        }
        else{
            (intersect = ghost & sizingRects->at(i));
        }

        //if an intersection exists
        if (intersect.area() > 0) {
            amountIntersected += intersect.area(); //calculate how much of the ghost is filled
            intersected = true;
        }
        //else there is no intersection with this ghost just skip
        else {
            continue;
        }

        //in a top left coordinate system

        Rect rect = sizingRects->at(i); //temporary var for readability

        //top left x
        if (rect.tl().x < ghost.tl().x) {
            double val = ghost.tl().x + ((rect.tl().x - ghost.tl().x) * sizeRatio);
            ghost = Rect(Point((int)val, ghost.tl().y), Point(ghost.br().x, ghost.br().y));
        }
        //top left y
        if (rect.tl().y < ghost.tl().y) {
            double val = ghost.tl().y + ((rect.tl().y - ghost.tl().y) * sizeRatio);
            ghost = Rect(Point(ghost.tl().x, (int)val), Point(ghost.br().x, ghost.br().y));
        }
        //bottom right x
        if (rect.br().x > ghost.br().x) {
            double val = ghost.br().x + ((rect.br().x - ghost.br().x) * sizeRatio);
            ghost = Rect(Point(ghost.tl().x, ghost.tl().y), Point((int)val, ghost.br().y));
        }
        //bottom right y
        if (rect.br().y > ghost.br().y) {
            double val = ghost.br().y + ((rect.br().y - ghost.br().y) * sizeRatio);
            ghost = Rect(Point(ghost.tl().x, ghost.tl().y), Point(ghost.br().x, (int)val));
        }
    }

    //increment time alive
    onScreenTime++;

    //if no rects intersected the ghost decrease area (shrink) or if thresh of intersection hasn't been reached
    if (!intersected || ((double)amountIntersected / (double) ghost.area()) < shrinkThresh) {
        return decreaseArea();
    }

    //if intersections valid then don't delete
    return false;
}

//decreases the size of the ghost, returns true to prompt delete
//top-left origin co-ordinate system
bool Ghost::decreaseArea(){
    //x & y distance between top-left and bottom-right
    int x = ghost.br().x - ghost.tl().x;
    int y = ghost.br().y - ghost.tl().y;

    //if the rectangle is tiny then return false to delete
    if(x < deleteThresh || y < deleteThresh){
        return true;
    }

    //shrink the ghost inwards
    double val = ghost.tl().x + (x * shrinkRatio);
    ghost = Rect(Point((int)val, ghost.tl().y), Point(ghost.br().x, ghost.br().y));

    val = ghost.tl().y + (y * shrinkRatio);
    ghost = Rect(Point(ghost.tl().x, (int)val), Point(ghost.br().x, ghost.br().y));

    val = ghost.br().x - (x * shrinkRatio);
    ghost = Rect(Point(ghost.tl().x, ghost.tl().y), Point((int)val, ghost.br().y));

    val =  ghost.br().y - (y * shrinkRatio);
    ghost = Rect(Point(ghost.tl().x, ghost.tl().y), Point(ghost.br().x, (int)val));

    return false;
}






