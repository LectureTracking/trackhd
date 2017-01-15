//
// Created by Charles on 2016/09/05.
//

#include "../headers/BoardDetection.h"

using namespace cv;
using namespace std;

Mat src;
Mat src_gray;
Mat thresh_img;
int thresh = 100;
RNG rng(12345);

bool debug = true;
bool showContours = false;
bool showCanny = false;
int waitTime = 1;

int keyPointChangeThresh = 150;
int leftKeyPointInitial = 0;
int rightKeyPointInitial = 0;
Rect rightHalf;
Rect leftHalf;
bool initialLoop = true;

double boardFoundThresh = 0.4;

VideoWriter boardVid;

Mat debugFrame;

IlluminationCorrection illuminationCorrection;

BoardDetection::BoardDetection()
{



    boardVid.open("boardDebug.flv", CV_FOURCC('F', 'L', 'V', '1'), 1, cv::Size(3840, 2160), 1);


}

///----------------------------//
/// Main method to find boards //
///----------------------------//
void BoardDetection::extractBoards(std::vector<Mat> &frames, PersistentData &pD)
{
    if(debug){
        namedWindow("Debug Viewer", WINDOW_NORMAL);
        resizeWindow("Debug Viewer", 1280, 720);
    }


    //Initialise parameters needed
    Size sizeOfFrames = pD.videoDimension;
    leftHalf = Rect(Point(0, 0), Point(sizeOfFrames.width / 2, sizeOfFrames.height));
    rightHalf = Rect(Point(sizeOfFrames.width / 2, 0), Point(sizeOfFrames.width, sizeOfFrames.height));

    vector<BoardDetection::BoardRect> boardRectangles; //Store all board rectangles found
    vector<BoardDetection::BoardRect> boardColumnRectangles;

    //create GUI windows
    if (debug)
    {
        if (showCanny)
        {
            namedWindow("Canny", WINDOW_NORMAL);
            resizeWindow("Canny", 1080, 720);
        }

        if (showContours)
        {
            namedWindow("Contours", WINDOW_NORMAL);
            resizeWindow("Contours", 1080, 720);
        }

    }

    Rect cropRegion;
    Rect finalCrop;


    //Find board crop region
    for (int i = 0; i < frames.size(); i += pD.boardDetectionSkipFrames)
    {
        if (i >= frames.size())
        {
            break;
        }
        Mat frame = frames[i];
        debugFrame = frames[i];
        //illuminationCorrection.histogramNormalisation(frame);

        //Find the cropping area
        boardRectangles.clear();
        boardColumnRectangles.clear();
        findRectangles(frame, cropRegion, boardRectangles, boardColumnRectangles);

        int leftK = 0;
        int rightK = 0;

        bool projectorUsedLeft = false;
        bool projectorUsedRight = false;

        bool leftBoardUsed = false;
        bool rightBoardUsed = false;

        Scalar color;

        for (int a = 0; a < boardRectangles.size(); a++)
        {
            BoardDetection::BoardRect b = boardRectangles.at(a);

            switch (b.boardType)
            {
                case BLACKBOARD:
                    if (isContained(b.r, rightHalf))
                    {
                        //Board is on the right
                        rightK += b.numFeatures;
                    } else if (isContained(b.r, leftHalf))
                    {
                        //Board is on the left
                        leftK += b.numFeatures;
                    }

                    color = Scalar(255, 0, 0);
                    rectangle(debugFrame, b.r.tl(), b.r.br(), color, 2, 8, 0);

                    putText(debugFrame, ("Blackboard " + to_string(a) + " (" + to_string(b.numFeatures) + ")"),
                            Point(b.r.x, b.r.y), FONT_HERSHEY_PLAIN, 4.0,
                            color, 4.0);
                    drawKeypoints(debugFrame(b.r), b.features, debugFrame(b.r), Scalar::all(-1),
                                  DrawMatchesFlags::DEFAULT);


                    break;
                case PROJECTORSCREEN:
                    if (isContained(b.r, rightHalf))
                    {
                        projectorUsedRight = true;
                    } else if (isContained(b.r, leftHalf))
                    {
                        projectorUsedLeft = true;
                    }

                    color = Scalar(0, 255, 0);
                    rectangle(debugFrame, b.r.tl(), b.r.br(), color, 2, 8, 0);

                    putText(debugFrame, ("Projector Screen " + to_string(a) + " (" + to_string(b.numFeatures) + ")"),
                            Point(b.r.x, b.r.y), FONT_HERSHEY_PLAIN, 4.0,
                            color, 4.0);
                    drawKeypoints(debugFrame(b.r), b.features, debugFrame(b.r), Scalar::all(-1),
                                  DrawMatchesFlags::DEFAULT);
            }
        }

        if (initialLoop)
        {
            leftKeyPointInitial = leftK;
            rightKeyPointInitial = rightK;
            initialLoop = false;

        } else
        {
            //Now check if change in number of keypoints is significant
            if (leftK > leftKeyPointInitial + keyPointChangeThresh)
            {
                //Left board used
                leftBoardUsed = true;
                cout << "Left used" << endl;

            }
            if (rightK > rightKeyPointInitial + keyPointChangeThresh)
            {
                //right board used
                rightBoardUsed = true;
                cout << "Right used" << endl;
            }


            //Now apply this update back to the metaframe
            pD.metaFrameVector.push_back(
                    MetaFrame(true, leftBoardUsed, rightBoardUsed, projectorUsedLeft, projectorUsedRight));

        }


        for (int a = 0; a < boardColumnRectangles.size(); a++){
            Scalar color = Scalar(0, 0, 255);
            Rect boardRect = boardColumnRectangles.at(a).r;

            //drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            rectangle(debugFrame, boardRect.tl(), boardRect.br(), color, 2, 8, 0);

            if(isContained(boardRect, leftHalf)){
                if(leftBoardUsed){
                    putText(debugFrame,
                            ("Board Column " + to_string(a) + " - USED"),
                            Point(boardRect.x, boardRect.y), FONT_HERSHEY_PLAIN, 4.0,
                            color, 4.0);
                } else{
                    putText(debugFrame,
                            ("Board Column " + to_string(a)),
                            Point(boardRect.x, boardRect.y), FONT_HERSHEY_PLAIN, 4.0,
                            color, 4.0);
                }
            } else{
                if(rightBoardUsed){
                    putText(debugFrame,
                            ("Board Column " + to_string(a) + " - USED"),
                            Point(boardRect.x, boardRect.y), FONT_HERSHEY_PLAIN, 4.0,
                            color, 4.0);
                } else{
                    putText(debugFrame,
                            ("Board Column " + to_string(a)),
                            Point(boardRect.x, boardRect.y), FONT_HERSHEY_PLAIN, 4.0,
                            color, 4.0);
                }
            }

        }



        ///cout << "Left Total Features: " << leftKeyPointInitial << endl;
        ///cout << "Right Total Features: " << rightKeyPointInitial << endl;
        ///cout << "Left Features: " << leftK << endl;
        ///cout << "Right Features: " << rightK << endl;
        //If we find an area big enough we assume the boards are contained in the crop region
        if (cropRegion.area() > int(boardFoundThresh * sizeOfFrames.area()) && !pD.boardsFound)
        {
            finalCrop = cropRegion;
            pD.boardsFound = true;
            pD.boardCropRegion = cropRegion;
            cout << "Board was found" << endl;
        } else
        {
            //If the new area found is bigger than current crop area --> Update current crop area to new
            if (cropRegion.area() > pD.boardCropRegion.area())
            {
                // pD.boardCropRegion = cropRegion;
            }
            //cerr << "Crop NOT found!!! " << "The current crop region is " << cropRegion.area() << " and the desired area is "  << int(boardFoundThresh*sizeOfFrames.area()) <<  endl;
        }


        if(pD.boardsFound){
            Scalar color = Scalar(0, 0, 0);
            rectangle(debugFrame, pD.boardCropRegion.tl(),pD.boardCropRegion.br(), color, 2, 8, 0);

            putText(debugFrame, "Final Crop Region",
                    Point(pD.boardCropRegion.x+1200, pD.boardCropRegion.y), FONT_HERSHEY_PLAIN, 4.0,
                    color, 4.0);
        }


        if(debug){
            imshow("Debug Viewer", debugFrame);
            waitKey(waitTime);
            //Output here
            boardVid.write(debugFrame);

        }
    }
}

///-------------------------------------------//
/// Main method to find rectangles in a frame //
///-------------------------------------------//
void BoardDetection::findRectangles(cv::Mat &frame, Rect &cropArea, vector<BoardDetection::BoardRect> &boardRects,
                                    vector<BoardDetection::BoardRect> &boardColumnRects)
{


    src = frame;
    /// Convert image to gray and blur it
    cvtColor(src, src_gray, CV_BGR2GRAY);
    blur(src_gray, src_gray, Size(3, 3));



    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //cv::Mat grayOverlay = src_gray.clone();


    //Threshold the image to detect blackboards easier
    threshold(src_gray, thresh_img, 200, 255, THRESH_BINARY);


    /// Detect edges using canny
    Canny(src_gray, canny_output, thresh, thresh * 2, 3);

    if (debug && showCanny)
    {
        imshow("Canny", canny_output);
    }
    /// Find contours
    findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    /// Draw contours
    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); i++)
    {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
    }


    /// Show in a window

    if (debug && showContours)
    {
        imshow("Contours", drawing);
    }

    //Bound contours with rectangle
    //Vector to store bounding rectangles
    vector<Rect> rects;
    BoardDetection::boundContoursWithRectangles(contours, rects);

    //Rect cropArea;
    vector<Rect> possibleBoardColumnsVec;


    BoardDetection::findBoards(rects, possibleBoardColumnsVec, boardRects);
    BoardDetection::findBoardColumns(possibleBoardColumnsVec, boardColumnRects);
    BoardDetection::findAreaToSegment(boardColumnRects, cropArea, boardRects);

    BoardDetection::removeOverlappingRectangles(boardRects);
    BoardDetection::removeOverlappingRectangles(boardColumnRects);

}

void BoardDetection::boundContoursWithRectangles(vector<vector<Point> > contours, vector<Rect> &vR)
{

    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());


    for (int i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
    }

    vR = boundRect;
}

//Simple method to check if a value is contained in a vector of values
bool BoardDetection::checkContainment(vector<int> &v, int val)
{

    bool found = false;
    for (int i = 0; i < v.size(); i++)
    {
        if (v.at(i) == val)
        {
            found = true;
        }
    }

    return found;
}

void BoardDetection::findBoards(std::vector<cv::Rect> &allRectangles, std::vector<cv::Rect> &possibleBoardColumns,
                                std::vector<BoardDetection::BoardRect> &boards)
{

    int boardIdCounter = 0;
    int projectorIdCounter = 0;


    ///cout << "In findBoards() method" << endl;
    vector<Rect> rectanglesToKeep;
    ///Loop over all rectangles
    for (int i = 0; i < allRectangles.size(); i++)
    {
        Rect r = allRectangles.at(i);
        //Ignore really small rectangles
        if (r.width > 400 && r.height > 300 && r.height <= 720)
        {
            ///cout << "Not a very small rectangle" << endl;
            ///cout << "Aspect ratio: " << calculateAspectRatio(r) << endl;
            ///cout << "IsDark? " << isDark(r) << endl;
            //Based on aspect ratio, classify as Projector screen or blackboard
            if (calculateAspectRatio(r) >= 165 && calculateAspectRatio(r) <= 200 && isDark(r))
            {
                //Close to a 16:9 ratio and classified as being a dark area... So assume blackboard
                /// cout << "Blackboard found" << endl;
                rectanglesToKeep.push_back(r);

                BoardRect b(boardIdCounter, r, countFeatures(src_gray(r)), BLACKBOARD);
                boards.push_back(b);
                boardIdCounter++;

            } else if (calculateAspectRatio(r) >= 100 && calculateAspectRatio(r) <= 145 && !isDark(r))
            {
                //Close to a 4:3 ratio and classified as being a light area... So assume projector screen
                ///cout << "Projector screen found" << endl;
                //rectanglesToKeep.push_back(r);

                BoardRect p(projectorIdCounter, r, countFeatures(src_gray(r)), PROJECTORSCREEN);
                boards.push_back(p);

                projectorIdCounter++;
            } else
            {
                ///cout << "Something else" << endl;

            }
        } else if (r.width > 400 && r.height > 300 && r.height > 1000)
        {
            //Possibly the board column
            ///cout << "Possibly the board column" << endl;
            possibleBoardColumns.push_back(r);
        }
    }
}

void BoardDetection::removeOverlappingRectangles(vector<BoardDetection::BoardRect> &boards)
{
///-------------------------------------------///
    /// STAGE 1 - Remove all contained rectangles ///
    ///-------------------------------------------///
    vector<int> toRemove;
    //Remove rectangles contained in another
    for (int i = 0; i < boards.size(); i++)
    {

        Rect r1 = boards.at(i).r;
        for (int j = i + 1; j < boards.size(); j++)
        {

            //cout << "Comparing rect " << i << " with " << j << endl;

            Rect r2 = boards.at(j).r;
            Rect r3 = r1 & r2;

            if (r3.area() > 0)
            {
                if (r3.area() == r2.area())
                {
                    //cout << "r2 is inside r1" << endl;
                    toRemove.push_back(boards.at(i).ID);
                } else if (r3.area() == r1.area())
                {
                    //cout << "r1 is inside r2" << endl;
                    toRemove.push_back(boards.at(j).ID);
                } else
                {
                    //cout << "Overlapping Rectangles" << endl;
                    if (r1.area() > r2.area())
                    {
                        toRemove.push_back(boards.at(i).ID); //Remove smaller rectangle R2
                    } else
                    {
                        toRemove.push_back(boards.at(j).ID);//Remove smaller rectangle R1
                    }
                }
            } else
            {
                //cout << "Non-overlapping Rectangles" << endl;

            }
        }
    }

    vector<BoardDetection::BoardRect> toKeep;
    for (int i = 0; i < boards.size(); i++)
    {
        if (!checkContainment(toRemove, boards.at(i).ID))
        {
            toKeep.push_back(boards.at(i));
        }

    }

    boards.clear();
    boards = move(toKeep);

}

void BoardDetection::findBoardColumns(std::vector<cv::Rect> &allRectangles, std::vector<BoardRect> &boardColumnsR)
{

    ///-------------------------------------------///
    /// STAGE 1 - Remove all contained rectangles ///
    ///-------------------------------------------///
    vector<int> toRemove;
    //Remove rectangles contained in another
    for (int i = 0; i < allRectangles.size(); i++)
    {

        Rect r1 = allRectangles[i];
        for (int j = i + 1; j < allRectangles.size(); j++)
        {

            //cout << "Comparing rect " << i << " with " << j << endl;

            Rect r2 = allRectangles[j];
            Rect r3 = r1 & r2;

            if (r3.area() > 0)
            {
                if (r3.area() == r2.area())
                {
                    //cout << "r2 is inside r1" << endl;
                    toRemove.push_back(j);
                } else if (r3.area() == r1.area())
                {
                    //cout << "r1 is inside r2" << endl;
                    toRemove.push_back(i);
                } else
                {
                    //cout << "Overlapping Rectangles" << endl;
                    if (r1.area() > r2.area())
                    {
                        toRemove.push_back(j); //Remove smaller rectangle R2
                    } else
                    {
                        toRemove.push_back(i); //Remove smaller rectangle R1
                    }
                }
            } else
            {
                //cout << "Non-overlapping Rectangles" << endl;
            }
        }
    }

    ///---------------------------------------------------///
    /// STAGE 2 - Keep only possible candidate rectangles ///
    ///---------------------------------------------------///
    //Evaluate all rectangles and keep only candidates that pass a certain criteria
    //vector<Rect> filtered;
    int boardColumnId = 0;
    for (int i = 0; i < allRectangles.size(); i++)
    {

        if (!checkContainment(toRemove, i))
        {
            Rect t = allRectangles.at(i);


            if (t.width > 400 && t.width < 2000 && t.height > 500)
            {
                //Check to see if the rectangle is not unreasonably small
                //filtered.push_back(t);
                BoardRect bc(boardColumnId, t, countFeatures(src_gray(t)), BOUNDING);
                boardColumnsR.push_back(bc);
                boardColumnId++;
            }
        }
    }

    //allRectangles.clear();
    //allRectangles = move(filtered);
}

void BoardDetection::findAreaToSegment(std::vector<BoardRect> &boardColumns, Rect &finalCrop,
                                       std::vector<BoardRect> &Boards)
{
    ///-------------------------------------///
    /// STAGE 1 - Decide on area to segment ///
    ///-------------------------------------///
    //Bound overall crop region
    int minX = INT_MAX, minY = INT_MAX;
    int maxX = INT_MIN, maxY = INT_MIN;

    for (BoardRect boardRect:boardColumns)
    {

        if (boardRect.r.tl().x < minX)
        {
            minX = boardRect.r.tl().x;
        }
        if (boardRect.r.tl().y < minY)
        {
            minY = boardRect.r.tl().y;
        }
        if (boardRect.r.br().x > maxX)
        {
            maxX = boardRect.r.br().x;
        }
        if (boardRect.r.br().y > maxY)
        {
            maxY = boardRect.r.br().y;
        }
    }

    //Extend further incase boards do extend
    for (BoardRect boardRect:Boards)
    {

        if (boardRect.r.tl().x < minX)
        {
            minX = boardRect.r.tl().x;
        }
        if (boardRect.r.tl().y < minY)
        {
            minY = boardRect.r.tl().y;
        }
        if (boardRect.r.br().x > maxX)
        {
            maxX = boardRect.r.br().x;
        }
        if (boardRect.r.br().y > maxY)
        {
            maxY = boardRect.r.br().y;
        }
    }

    ///-----------------------------------///
    /// STAGE 2 - Set overall crop region ///
    ///-----------------------------------///
    finalCrop = Rect(Point(minX, minY), Point(maxX, maxY));
}

int BoardDetection::calculateAspectRatio(cv::Rect &r)
{
    //Returns 177 for 16:9 and 133 for 4:3
    // cout << "Width = " << r.width << " Height = " << r.height << " Aspect Ratio = " << (double) (r.width / r.height)<< endl;
    return (int) ((r.width / r.height) * 100);
}

bool BoardDetection::isDark(cv::Rect &r)
{

    Mat tmpImg = (thresh_img(r));
    int TotalNumberOfPixels = tmpImg.rows * tmpImg.cols;
    int ZeroPixels = TotalNumberOfPixels - countNonZero(tmpImg);
    float percent = ((float) ZeroPixels / (float) TotalNumberOfPixels) * 100;
    ///cout << "Percentage black: " << percent << endl;
    return percent > 75;
}

std::vector<KeyPoint> BoardDetection::countFeatures(Mat cropImg)
{

    //Perform sift function on image


    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;

    Ptr<cv::xfeatures2d::SURF> detector = xfeatures2d::SURF::create(minHessian);

    std::vector<KeyPoint> keypoints_1;

    detector->detect(cropImg, keypoints_1);


    /* //-- Draw keypoints
     Mat img_keypoints_1;

     drawKeypoints( cropImg, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

     //-- Show detected (drawn) keypoints
     imshow("Keypoints 1", img_keypoints_1 );
     imwrite("KeyPoints.png", img_keypoints_1);

     waitKey(waitTime);*/

    return keypoints_1;
}

bool BoardDetection::isContained(cv::Rect r1, cv::Rect r2)
{

    Rect r3 = r1 & r2;
    //Is r1 contained in r2?
    return r3.area() == r1.area();
}




//Detect writing (http://stackoverflow.com/questions/35320409/estimate-white-background)
/*
 * #include <opencv2/opencv.hpp>
#include <opencv2/photo.hpp>
using namespace cv;

void findText(const Mat3b& src, Mat1b& mask)
{
    // Convert to grayscale
    Mat1b gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // Compute gradient magnitude
    Mat1f dx, dy, mag;
    Sobel(gray, dx, CV_32F, 1, 0);
    Sobel(gray, dy, CV_32F, 0, 1);
    magnitude(dx, dy, mag);

    // Remove low magnitude, keep only text
    mask = mag > 10;

    // Apply a dilation to deal with thick text
    Mat1b K = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    dilate(mask, mask, K);
}

int main(int argc, const char * argv[])
{
    Mat3b img = imread("path_to_image");

    // Segment white
    Mat1b mask;
    findText(img, mask);

    // Show intermediate images
    Mat3b background = img.clone();
    background.setTo(0, mask);

    Mat3b foreground = img.clone();
    foreground.setTo(0, ~mask);

    // Apply inpainting
    Mat3b inpainted;
    inpaint(img, mask, inpainted, 21, CV_INPAINT_TELEA);

    imshow("Original", img);
    imshow("Foreground", foreground);
    imshow("Background", background);
    imshow("Inpainted", inpainted);
    waitKey();

    return 0;
}
 */