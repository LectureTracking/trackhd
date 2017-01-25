//
// Created by Fudge on 2016/09/05.
//

#include "MovementDetection.h"
#include <iostream>
#include <opencv/cv.hpp>

using namespace cv;
using namespace std;


VideoWriter maxS;




MovementDetection::MovementDetection(string vidLocation, vector<Rect>* lect) {

    //set vid directory and window name
    setVideoDir(vidLocation);
    setShowLabel("MyVideo");

    maxS.open("maxS.flv", CV_FOURCC('F', 'L', 'V', '1'), 7, cv::Size(3840, 2160), 1);

    //create the inputVideo object
    VideoCapture inputVideo(videoDir);
    if(!inputVideo.isOpened()){
        //error opening the video input
        cerr << "Unable to open video file" << endl;
        return;
    }

    // -------------------- //

    //get input video settings and apply to output video
    double fps = inputVideo.get(CV_CAP_PROP_FPS);
    Size S = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    if(showFileInfo){
        cout << "File Information" << endl;
        cout << "Input file location: " << vidLocation << endl;
        cout << "Video FPS: " << fps << endl;
        cout << "Video Frame Count: " << inputVideo.get(CV_CAP_PROP_FRAME_COUNT) << endl;
        cout << endl;
    }

    //if we get an acceptable fps value
    //certain file formats (e.g flv) sometimes have no fps when read with this OpenCV version
    if(fps > 5 && fps < 60){
        validFps = true;
    }
    else{
        cerr << "Invalid FPS video from file, removing FPS tracking." << endl;
    }

    center.x = S.width / 2; //center of the screen X value
    center.y = S.height / 2; //center of the screen Y value

    //if create window
    if(drawFrameStatus) {
        namedWindow(showLabel, CV_WINDOW_NORMAL);
    }

    //frame read dependent processing variable
    clock_t processTime = clock();

    int emptyFrameCount = 0;

    //preliminarily read previous frame so we can do absDiff
    if(!inputVideo.read(prevFrame)){
        cerr << "Unable to read first frame." << endl;
        cerr << "Exiting..." << endl;
        return;
    }

    //convert initial to grayscale
    cvtColor(prevFrame, prevFrame, CV_BGR2GRAY);

    //if true read frame as the next frame, if false read prevFrame as
    // the next frame, this is to avoid copying and moving around data
    bool swap = true;

    //number of frames to skip on successive reads
    int frameSkip = frameSkipReset;

    // -------------------- //

    //frame information
    long frameNumber = 0;

    //y rect ignore zones
    double lowScreenBar = S.height - (S.height * lowScreenThresh); //actual pixels from the bottom
    double highScreenBar = (S.height * highScreenThresh); //actual pixels from the top

    //number of frames before onScreenTime subtract
    int ghostFrameCount = ghostResetValue;

    vector<Ghost> ghostRects; //vector of ghost rectangle objects

    // -------------------- //

    //read input data
    while(true){

        onLoopReset();

        frameNumber++;

        //require 2 frames loaded to do absdiff
        if(swap) {
            swap = !swap;
            //end of video
            if (!inputVideo.read(frame)) {
                break;
            }
            //greyscale applied to frame
            cvtColor(frame, frame, CV_BGR2GRAY);
        }
        else{
            swap = !swap; //make true
            //end of video
            if (!inputVideo.read(prevFrame)){
                break;
            }
            //grayscale applied to prevFrame
            cvtColor(prevFrame, prevFrame, CV_BGR2GRAY);
        }

        // -------------------- //

        //perform frame skips
        if(frameSkip == 0){
            frameSkip = frameSkipReset;
        }
        else{
            frameSkip--;
            continue;
        }

        //apply absolute difference background subtraction
        absdiff(frame, prevFrame, frameAbsDiff);

        //perform thresholding
        threshold(frameAbsDiff, frameThresh, 25, 255, 0);

        //get structuring element for morphological filters
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(0, 0));

        //perform morphological dilation
        dilate(frameThresh, frameMorph, element);
        blur(frameMorph, frameBlur, Size(15, 15), Point(0, 0), BORDER_DEFAULT);

        // -------------------- //

        //find contours
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        findContours(frameBlur, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

        //random number generator definition
        RNG rng(12345);

        //remove small or inconsequential contours (should help with camera refocus and noise)
        for(int i = 0; i < contours.size(); i++){
            if(contours[i].size() < contourMinSize){
                contours.erase(contours.begin() + i);
                i--;
            }
        }

        //create bounding rectangles out of contours
        vector<Rect> boundingRects;
        for(int i = 0; i < contours.size(); i++){
            boundingRects.push_back(cv::boundingRect(contours[i]));
        }

        // -------------------- //

        //remove rectangles below a certain threshold of the screen or above a certain threshold of the screen
        //in a top-left origin co-ordinate system
        for(int i = (int)boundingRects.size() - 1; i >= 0; i--){
            if(boundingRects[i].tl().y >= lowScreenBar || boundingRects[i].br().y <= highScreenBar){
                boundingRects.erase(boundingRects.begin() + i);
            }
        }

        //remove rectangles with a very wide aspect ratio aspectR:1
        //this is specifically for boards and projectors
        for(int i = 0; i < boundingRects.size(); i++){
            int xDiff = (boundingRects[i].br().x - boundingRects[i].tl().x);
            int yDiff = (boundingRects[i].br().y - boundingRects[i].tl().y);
            if((double)xDiff / (double)yDiff > aspectR){
                boundingRects.erase(boundingRects.begin() + i);
                i--;
            }
        }

        // -------------------- //

        //perform overlap and proximity check to ensure nearby or overlapping rectangles are grouped
        overlapProximityLoop(&boundingRects);

        //if any massive shapes are found (usually on refocusing camera) just purge the frame of rects and create a default rect
        if(oversizeCheck(&boundingRects, S.width, S.height)){
            emptyFrameCount++;
            boundingRects.clear();
            boundingRects.push_back(Rect(Point(center.x - 50, center.y - 50), Point(center.x + 50, center.y + 50)));
            if(showFrameStatus) {
                //status information
                cout << "Massive rects found at: " << frameNumber << endl;
                cout << "No usable information!" << endl;
                cout << "Generating center rectangle" << endl;
                cout << "-------------------------" << endl << endl;
            }
        }

        //if there are no bounding rects left then generate a center rect
        if(boundingRects.size() == 0){
            emptyFrameCount++;
            //if no bounding rects, push_back a default rect
            boundingRects.push_back(Rect(Point(center.x - 50, center.y - 50), Point(center.x + 50, center.y + 50)));
            if(showFrameStatus) {
                //status information
                cout << "Empty Frame: " << frameNumber << endl;
                cout << "No rectangles found!" << endl;
                cout << "Generating center rectangle" << endl;
                cout << "-------------------------" << endl << endl;
            }
        }

        // -------------------- //

        //add new ghosts (rects that don't intersect anywhere)
        addNewGhosts(&ghostRects, &boundingRects);

        //push overlapping ghosts together
        ghostOverlapCheck(&ghostRects);

        //perform ghost resize checks
        for(int i = 0; i < ghostRects.size(); i++) {
            if(ghostRects.at(i).resize(&boundingRects)){
                ghostRects.erase(ghostRects.begin() + i);
                i--;
            }
        }

        //push overlapping ghosts together
        ghostOverlapCheck(&ghostRects);

        //check if rectangles within a ghost are a certain distance away, if so split into 2 or more ghosts
        ghostSplitCheck(&ghostRects, &boundingRects);

        // -------------------- //

        //after a certain number of frames subtract ghost counts
        ghostFrameCount--;
        if(ghostFrameCount == 0){
            for(int i = 0; i < ghostRects.size(); i++){
                ghostRects[i].subOnScreenTime(ghostResetAmount);
            }

            ghostFrameCount = ghostResetValue;
        }

        // -------------------- //

        //add rectangles & ghosts to memory
        memoryRects.push_back(boundingRects);
        memoryGhosts.push_back(ghostRects);

        // -------------------- //
        
        //just to visualize colours//
        cvtColor(frameBlur, frameBlur, CV_GRAY2RGB);

        //display over the original frame
        frameDisplay = frame.clone();
        cvtColor(frameDisplay, frameDisplay, CV_GRAY2RGB);

        if(drawFrameStatus) {
            //draw contours
            if(drawSettings[0]) {
                for (int i = 0; i < contours.size(); i++) {
                    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
                    drawContours(frameDisplay, contours, i, color, 2, 8, hierarchy, 0, Point());
                }
            }

            //draw rectangles around contours
            if(drawSettings[1]) {
                for (int i = 0; i < boundingRects.size(); i++) {
                    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
                    rectangle(frameDisplay, boundingRects[i].tl(), boundingRects[i].br(), color, 2, 8, 0);
                }
            }

            //draw ghosts in frame
            if(drawSettings[2]) {
                for (int i = 0; i < ghostRects.size(); i++) {
                    Scalar color = Scalar(0, 0, 0);
                    rectangle(frameDisplay, ghostRects[i].tl(), ghostRects[i].br(), color, 6, 8, 0);
                }
            }

            //draw ghost on screen time to ghosts
            if(drawSettings[3]) {
                for (int i = 0; i < ghostRects.size(); i++) {
                    Scalar color = Scalar(0, 0, 0);

                    ss << ghostRects.at(i).getOnScreenTime();
                    frameText = ss.str();
                    ss.str("");

                    Rect rect = ghostRects.at(i).getGhost();
                    Point rectCent(rect.x + (rect.width / 3), rect.y + (rect.height / 2));

                    putText(frameDisplay, frameText, rectCent, FONT_HERSHEY_PLAIN, 3, color, 3, 8, false);
                }
            }
        }


        if(drawFrameStatus) {
            //add values to print to frame
            if(drawSettings[4]){
                if(validFps){
                    addTextToFrame(&frameDisplay, "", convertFrameToTime(frameNumber, fps));
                }
            }
            if(drawSettings[5]){
                addTextToFrame(&frameDisplay, "Frame: ", frameNumber);
            }
            if(drawSettings[6]){
                addTextToFrame(&frameDisplay, "Rectangles: ", boundingRects.size());
            }

            //show processing over grayscale frame
            imshow(showLabel, frameDisplay);
            maxS.write(frameDisplay);


        }

        if(showFrameStatus) {
            //status information
            cout << "Frame: " << frameNumber << endl;
            if(validFps) {
                cout << convertFrameToTime(frameNumber, fps) << endl;
            }
            cout << "Number of rectangles: " << boundingRects.size() << endl;
            cout << "Number of ghosts: " << ghostRects.size() << endl;
            cout << "-------------------------" << endl << endl;
        }

        //waitKey(will step through at x milliseconds rate)
        //if 0 waitKey will step through when key is pressed
        if (waitKey(1)) {
            continue;
        }
    }

    //push final frame on
    memoryGhosts.push_back((memoryGhosts.at(memoryGhosts.size() - 1)));
    memoryRects.push_back((memoryRects.at(memoryRects.size() - 1)));

    cout << "Final frame count: " << frameNumber << endl;

    inputVideo.release();

    processTime = clock() - processTime;

    cout << "Processing took: " << (processTime / 1000.0) << "s" << endl;

    cout << "Performing ghost adjustments!" << endl;

    // -------------------- //

    //time the post-processing
    clock_t time = clock();

    //begin the post-processing to find the lecturer from multiple ghosts

    //use the largest number calculated with a weight based on x-position favoring the center of the screen

    findLecturer(&memoryGhosts, &lecturer);

    //once main ghost has been found shift the ghost to match the avg of rectangles coordinates that
    //intersect it by a weighted average of intersection amount
    adjustLecturer(&lecturer, &memoryRects, lect);

    //decide to draw frame
    if(drawFrameStatus) {
        //print ghosts to frame and output
        Scalar color = Scalar(0, 0, 0); //ghost color
        showFramesMemoryRects(&memoryGhosts, color);
    }

    time = (clock() - time);

    cout << "Postprocessing took: " << (time) << "ms" << endl;

    cout << "Finished processing!" << endl;

    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_1_NORM_LECT.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_2_LECT_WAVE.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_3_LECT_PACE.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_4_LIGHTS.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_5_MOVE_BOARDS.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_6_MOVE_SCREEN.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_7_MOVE_SCREEN_2.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_8_OFF_ON.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_9_LECT_CROSS.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_10_BOTH_MOVE.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_11_BOTH_MOVE_2.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_12_RUNNING.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_13_THROWING.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_14_MULTI_STUDENT.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_15_MOVE_3.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_16_MOVE_CHAIRS.avi");
    //writeVideo(&lecturer, "Videos/Test Cases Output/CLIP_17_NO_MOVE.avi");
}

//calculates the distance between the closest two edge points of two rectangles
double MovementDetection::closestDistance(Rect a, Rect b){
    bool left = b.br().x < a.tl().x; //b left of a
    bool right = a.br().x < b.tl().x; //b right of a
    bool bottom = a.br().y < b.tl().y; //b below a
    bool top = b.br().y < a.tl().y; //b above a
    if(top and left)
        return euclideanDist(a.tl(), b.br());
    else if (left and bottom){
        return euclideanDist(Point(a.tl().x, a.br().y), Point(b.br().x, b.tl().y));
    }
    else if(bottom and right){
        return euclideanDist(a.br(), b.tl());
    }
    else if(right and top){
        return euclideanDist(Point(a.br().x, a.tl().y), Point(b.tl().x, b.br().y));
    }
    else if (left){
        return a.tl().x - b.br().x;
    }
    else if(right){
        return b.tl().x - a.br().x;
    }
    else if(bottom){
        return b.tl().y - a.br().y;
    }
    else{
        return a.tl().y - b.br().y;
    }
}

//helper function to calculate distance between two points
double MovementDetection::euclideanDist(Point p, Point q){
    Point diff = p - q;
    return cv::sqrt(diff.x * diff.x + diff.y * diff.y);
}

//check for overlapping rectangles and push them together
//return true if changes made
bool MovementDetection::overlapCheck(vector<Rect>* boundingRects) {
    //push overlapping rectangles together
    bool changeDetected;
    bool anyChange = false;

    do{
        //this array records changes in vector to be implemented
        bool deletionMark[boundingRects->size()];
        for(int i = 0; i < boundingRects->size(); i++){
            deletionMark[i] = false;
        }

        //if changes happen the rectangles need to be compared again
        //this shouldn't be a problem given that number of rectangles is mostly very small (1 - 20)
        changeDetected = false;

        /*
         * possible efficiency here, checking the area before complete occlusion
         * if the square is in another is their area > 0?? in which case throw the top
         * in as an if inside the other if
         */
        for(int i = 0; i < boundingRects->size(); i++){
            for(int k = 0; k < boundingRects->size(); k++){
                if(i != k){
                    //the intersection of 2 rectangles
                    Rect intersect = (boundingRects->at(i)) & (boundingRects->at(k));

                    //if first rectangle is completely inside second
                    if(intersect == boundingRects->at(i)){
                        deletionMark[i] = true; //mark for deletion
                        changeDetected = true;
                        continue;
                    }
                    //if rectangles overlap then just add them together
                    if(intersect.area() > 0){
                        boundingRects->at(i) = (boundingRects->at(i) | boundingRects->at(k)); //merge
                        deletionMark[k] = true;
                        changeDetected = true;
                    }

                }
            }
        }

        //delete marked cells
        //in reverse makes index easier to maintain
        for(int i = (int)boundingRects->size() - 1; i >= 0; i--){
            if(deletionMark[i]){
                boundingRects->erase(boundingRects->begin() + i);
            }
        }
        if(changeDetected)
            anyChange = true;
    }while(changeDetected);

    return anyChange;
}

//check for nearby rectangles and push them together
//return true if changes made
bool MovementDetection::proximityCheck(vector<Rect>* boundingRects){
    //returns if there were no changes at all
    bool anyChange = false;

    //check each rectangle against each other to see if distances between rectangles
    //are small enough to merge
    for(int i = 0; i < boundingRects->size(); i++) {
        for (int k = i; k < boundingRects->size(); k++) {
            if (i != k) {
                //if closer than threshold
                if(closestDistance(boundingRects->at(i), boundingRects->at(k)) < clusterThreshold){
                    boundingRects->at(i) = (boundingRects->at(i) | boundingRects->at(k)); //merge
                    //erase the rect and restart search
                    boundingRects->erase(boundingRects->begin() + k);
                    i = 0; k = 0; //reset the loop
                    anyChange = true;
                }
            }
        }
    }

    return anyChange;
}

//performs overlap and proximity checks until no more changes occur
void MovementDetection::overlapProximityLoop(vector<Rect>* boundingRects) {
    //keep going until no more changes
    while (true) {
        //if both return no changes
        if (!overlapCheck(boundingRects) && !proximityCheck(boundingRects)) {
            break;
        }
    }
}

//check if any massive rectangles exist in the scene
bool MovementDetection::oversizeCheck(vector<Rect>* boundingRects, int width, int height){
    for(int i = 0; i < boundingRects->size(); i++){
        if(boundingRects->at(i).area() > (width * height * massiveThreshold)){
            return true;
        }
    }
    return false;
}

//add text to frame function
void MovementDetection::addTextToFrame(Mat* frame, string str, auto value){
        ss << str << value;
        ++textCount;
        frameText = ss.str();
        putText(*frame, frameText, Point(30, 80 * textCount), FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 2, 8, false);
        ss.str("");
}

//values to reset on loop
void MovementDetection::onLoopReset(){
    textCount = 0;
}

//push overlapping ghosts together
void MovementDetection::ghostOverlapCheck(vector<Ghost>* ghostRects){
    //mark rects that need to be deleted
    bool deletionMark[ghostRects->size()];
    for(int i = 0; i < ghostRects->size(); i++){
        deletionMark[i] = false;
    }

    /*
     * check ghost intersection, if intersected then push together
     */
    for(int i = 0; i < ghostRects->size(); i++){
        for(int k = 0; k < ghostRects->size(); k++){
            if(i != k){
                //the intersection of 2 rectangles
                Rect intersect = (ghostRects->at(i).getGhost()) & (ghostRects->at(k).getGhost());

                //if first rectangle is completely inside second
                if(intersect == ghostRects->at(i).getGhost()){
                    deletionMark[i] = true; //mark for deletion
                    continue;
                }
                //if rectangles overlap then just add them together
                if(intersect.area() > 0){
                    ghostRects->at(i).reset((ghostRects->at(i).getGhost() | ghostRects->at(k).getGhost())); //merge
                    //mark largest time as time
                    if(ghostRects->at(k).getOnScreenTime() > ghostRects->at(i).getOnScreenTime()){
                        ghostRects->at(i).setOnScreenTime(ghostRects->at(k).getOnScreenTime());
                    }
                    deletionMark[k] = true;
                }

            }
        }
    }

    //remove marked to delete ghosts
    for(int i = (int)ghostRects->size() - 1; i >= 0; i--){
        if(deletionMark[i]) {
            ghostRects->erase(ghostRects->begin() + i);
        }
    }
}

//show frames using ghostRectangles
void MovementDetection::showFramesMemoryRects(vector<vector<Ghost>>* memoryGhosts, Scalar color){
    VideoCapture inputVideo(videoDir);
    if(!inputVideo.isOpened()){
        //error opening the video input
        cerr << "Unable to open video file" << endl;
        return;
    }

    //the frame that is rendered to file
    Mat printFrame;

    if(!inputVideo.read(printFrame)){
        cerr << "Unable to read first frame." << endl;
        cerr << "Exiting..." << endl;
        return;
    }

    int frameSkip = frameSkipReset;

    for(int i = 0; i < memoryGhosts->size() - 1;){

        if(!inputVideo.read(printFrame)){
            cout << "Moving to next stage!" << endl;
            return;
        }

        //perform frame skips
        if(frameSkip == 0){
            frameSkip = frameSkipReset;
            i++;
        }
        else{
            frameSkip--;
            continue;
        }

        //draw decided lecturer to frame from ghosts
        rectangle(printFrame, memoryGhosts->at(i).at(0).tl(), memoryGhosts->at(i).at(0).br(), color, 6, 8, 0);

        imshow(showLabel, printFrame);

        if (waitKey(1)) {
            continue;
        }
    }

    inputVideo.release();
}

//reposition ghost to centroid of squares
void MovementDetection::adjustLecturer(vector<Rect>* lecturer, vector<vector<Rect>>* memoryRects, vector<Rect>* newLect){

    for(int i = 0; i < lecturer->size(); i++){ //for each main ghost
        Rect tempGhost = lecturer->at(i);

        //our index array of rectangle that intersect validly
        vector<int> intersected;

        //for each rectangle at that frame
        for(int r = 0; r < memoryRects->at(i).size(); r++){

            Rect tempRect = memoryRects->at(i).at(r);

            Rect intersect = tempRect & tempGhost;

            //if there is any intersection
            if(intersect.area() > 0){
                //if the rect is much larger than ghost do nothing
                if(tempRect.area() / tempGhost.area() > 2){
                    continue;
                }
                else{
                    intersected.push_back(r);
                }
            }
        }

        //perform shifting operations on selected rects
        if(intersected.size() == 1){
            //take average 0.9 movement to the rect and make new (tl(), br()) rect
            int newTlX = (int)(shiftR * tempGhost.tl().x) + (int)(shiftRInv * memoryRects->at(i).at(intersected.at(0)).tl().x);
            int newBrX = (int)(shiftR * tempGhost.br().x) + (int)(shiftRInv * memoryRects->at(i).at(intersected.at(0)).br().x);

            Rect newRect(Rect(Point(newTlX, tempGhost.tl().y), Point(newBrX, tempGhost.br().y)));

            newLect->push_back(newRect);
        }
        else if(intersected.size() > 1){
            //average all intersecting rects and the ghost's coords
            int newTlX = tempGhost.tl().x;
            int newBrX = tempGhost.br().x;

            for(int j = 0; j < intersected.size(); j++){
                newTlX += memoryRects->at(i).at(intersected.at(j)).tl().x;
                newBrX += memoryRects->at(i).at(intersected.at(j)).br().x;
            }

            newTlX /= (intersected.size() + 1);
            newBrX /= (intersected.size() + 1);

            Rect newRect(Rect(Point(newTlX, tempGhost.tl().y), Point(newBrX, tempGhost.br().y)));

            newLect->push_back(newRect);
        }
    }
}

//add new ghosts (rectangles that don't intersect anywhere
void MovementDetection::addNewGhosts(vector<Ghost>* ghostRects, vector<Rect>* boundingRects){

    for(int i = 0; i < boundingRects->size(); i++){
        bool add = true;
        for(int g = 0; g < ghostRects->size(); g++){
            //check for overlap
            Rect intersect = ((ghostRects->at(g).getGhost()) & (boundingRects->at(i)));
            //if none break and move to next rect
            if(intersect.area() > 0){
                add = false;
                break;
            }
        }
        if(add){
            ghostRects->push_back(Ghost(boundingRects->at(i)));
        }
    }
}

//return timeOnScreen * (ratioOfDistFromCenter ^ 2) averaged between x and y
double MovementDetection::positionWeighting(Rect r, long onScreenTime){
    double distX = center.x - abs(r.x - center.x); //distance from center to rect
    double distY = center.y - abs(r.y - center.y); //distance from center to top of rect

    distX /= center.x; //get x ratio
    distY /= center.y; //get y ratio

    //pair of ratiod vals
    double ratioX;
    double ratioY;
    ratioX = onScreenTime * (distX * distX);
    ratioY = onScreenTime * (distY * distY);

    return (ratioX + ratioY) / 2;
}

//divide ghosts with multiple rects in it that are far apart
void MovementDetection::ghostSplitCheck(vector<Ghost>* ghostRects, vector<Rect>* boundingRects){

    bool ghostDelete[ghostRects->size()]; //assign all as false
    for(int i = 0; i < ghostRects->size(); i++){
        ghostDelete[i] = false;
    }

    vector<Ghost> newGhosts;

    for(int g = 0; g < ghostRects->size(); g++){
        vector<int> intersected;
        Ghost currGhost = ghostRects->at(g);
        for(int r = 0; r < boundingRects->size(); r++){

            //test intersection of rect with ghost
            Rect intersect = (currGhost.getGhost() & boundingRects->at(r));

            //check intersection & record
            if(intersect.area() > 0){
                intersected.push_back(r);
            }
        }

        //if more than one intersection
        //see if it needs to be split
        if(intersected.size() > 1){

            bool newGhostCheck[intersected.size()]; //if the index that intersected points to needs to be made new

            for(int m = 0; m < intersected.size(); m++){
                newGhostCheck[m] = false;
            }

            //check if minimum distance in ghost is below global allowed
            int minDist = currGhost.getGhost().width / 3;
            if(minDist < minSplitDist){
                minDist = minSplitDist;
            }

            //iterate over all intersection
            //use boolean checks to avoid creating duplicates
            for(int i = 0; i < intersected.size(); i++){
                for(int j = i + 1; j < intersected.size(); j++){
                    Rect a = boundingRects->at(intersected[i]);
                    Rect b = boundingRects->at(intersected[j]);

                    double dist;

                    //if there is intersection between the two rects then ignore
                    if((a & b).area() > 0){
                        continue;
                    }
                    //if completely above or below then ignore
                    if(a.br().y < b.tl().y || b.br().y < a.tl().y){
                        continue;
                    }

                    //if a is left of b
                    if(a.br().x < b.tl().x){
                        dist = b.tl().x - a.br().x;
                    }
                    //if b is left of a
                    else if(b.br().x < a.tl().x){
                        dist = a.tl().x - b.br().x;
                    }

                    //if x distance apart is large split them up
                    if(dist > minDist){
                        if(!newGhostCheck[i]){
                            newGhostCheck[i] = true;
                            newGhosts.push_back(Ghost(a, currGhost.getOnScreenTime()));
                        }

                        if(!newGhostCheck[j]) {
                            newGhostCheck[j] = true;
                            newGhosts.push_back(Ghost(b, currGhost.getOnScreenTime()));
                        }
                        ghostDelete[g] = true;
                    }
                }
            }
        }
        intersected.clear();
    }

    //delete all duplicate ghosts
    for(int i = ghostRects->size() - 1; i >= 0; i--){
        if(ghostDelete[i]){
            ghostRects->erase(ghostRects->begin() + i);
        }
    }

    //add all new ghosts onto ghostRects
    for(int i = 0; i < newGhosts.size(); i++){
        ghostRects->push_back(newGhosts.at(i));
    }
}

//find lecturer based on onScreenTime and position on the screen
void MovementDetection::findLecturer(vector<vector<Ghost>>* memoryGhosts, vector<Rect>* lecturer){

    cout << "\n--------------------\nMemoryGhosts size: " << memoryGhosts->size() << "\nalc--------------------\n" << endl;

    for(int i = 0; i < memoryGhosts->size(); i++){
        cout << "Ghost: " << i << " Size: " << memoryGhosts->at(i).size() << endl;
        if(memoryGhosts->at(i).size() == 0){
            if(i == 0){
                lecturer->push_back(Rect(Point(center.x - 50, center.y - 50), Point(center.x + 50, center.y + 50)));
                continue;
            }
            //if no ghost then set default previous
            lecturer->push_back(Rect(lecturer->at(i - 1)));
        }
        else {
            double largestVal = positionWeighting(memoryGhosts->at(i).at(0).getGhost(), memoryGhosts->at(i).at(0).getOnScreenTime());

            int largestIdx = 0;
            for (int k = 0; k < memoryGhosts->at(i).size(); k++) {
                if (memoryGhosts->at(i).at(k).getOnScreenTime() > largestVal) {
                    largestVal = positionWeighting(memoryGhosts->at(i).at(k).getGhost(), memoryGhosts->at(i).at(k).getOnScreenTime());
                    largestIdx = k;
                }
            }
            lecturer->push_back(memoryGhosts->at(i).at(largestIdx).getGhost());
        }
    }

    cout << "\n--------------------\nLecturer size: " << lecturer->size() << endl;
}

//convert frame number to string && hours:minutes:seconds
string MovementDetection::convertFrameToTime(long frameNumber, double fps){

    double currTime = frameNumber / fps;

    stringstream ss;
    int hours = (int)currTime / 3600;
    currTime -= (hours * 3600);
    int minutes = (int)currTime / 60;
    currTime -= (minutes * 60);
    int seconds = (int)currTime;

    ss << "Time: ";
    if(hours < 10)
        ss << "0" << hours;
    else
        ss << hours;
    ss << ":";
    if(minutes < 10)
        ss << "0" << minutes;
    else
        ss << minutes;
    ss << ":";
    if(seconds < 10)
        ss << "0" << seconds;
    else
        ss << seconds;

    return ss.str();
}

//copy lecturer values across
void MovementDetection::getLecturer(vector<Rect>* newLect){
    cout << "start of get lecturer" << endl;
    cout << "lecturer size: " << lecturer.size() << endl;
    for(int i = 0; i < lecturer.size(); i++){
        //cout << "loop: " << i << endl;
        newLect->push_back(lecturer.at(i));
    }
    cout << "end of get lecturer" << endl;
}

//write video with lecturer rectangle
void MovementDetection::writeVideo(vector<Rect>* lecturer, string outName){

    //create the inputVideo object
    VideoCapture inputVideo(videoDir);
    if(!inputVideo.isOpened()){
        //error opening the video input
        cerr << "Unable to open video file" << endl;
        return;
    }

    // -------------------- //

    //get input video settings and apply to output video
    double fps = inputVideo.get(CV_CAP_PROP_FPS);
    Size S = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    VideoWriter outVid(outName, CV_FOURCC('X', 'V', 'I', 'D'), fps, S, true);

    // -------------------- //

    if(!outVid.isOpened()){
        cerr << "Unable to open video write file" << endl;
        return;
    }

    Mat inFrame;

    Scalar color = Scalar(255, 0, 0);
    int count = 0;
    int frameReset = frameSkipReset;
    int frameNumber = 0;

    while(inputVideo.read(inFrame)){

        //draw lecturer onto frame
        rectangle(inFrame, lecturer->at(count).tl(), lecturer->at(count).br(), color, 2, 8, 0);

        outVid.write(inFrame);

        //write rect to each frame skipped as well as main
        if(frameReset != 0){
            frameReset--;
        }
        else{
            frameReset = frameSkipReset;
            if(count != lecturer->size() - 1){
                count++;
            }

        }

        frameNumber++;
    }

    cout << "Finished writing " << outName << endl;

}