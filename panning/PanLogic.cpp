//
// Created by Tanweer Khatieb on 2016/09/19.
//

#include "PanLogic.h"

using namespace std;
using namespace cv;

bool initialCrop = true;
bool boardDataActive = false;

//Cos function
long double PanLogic::smooth(double x)
{
    return (cos(x - (135)) + 1);
}

//Given a start and end point create a vector of rectangles that will essentially pan over that line
void PanLogic::smoothMove(int start, int end, int numFrames, bool right, Position panOffsetType,
                          std::vector<cv::Rect> &croppingRectangles)
{

    //This is the starting x position of the cropping rectangle
    //Todo - The (cropFrameWidth/2) is currently a crude way of centering the rectangle about the lecturers position (Which is assumed to be the (x,y) coordinates of his/her face.
    //TODO - This needs to be updated depending on whether the lecturer needs to be place in [LEFT; CENTER; RIGHT] of the frame and depends on board usage and how this will be handled

    if (initialCrop)
    {
        setPan(start, currentPan);
        initialCrop = false;
    }


    if (!checkIfPanRequired(start, end) && numFrames > 59)
    {

        switch (panOffsetType)
        {
            case LEFT:
                end -= (cropFrameWidth / 2) - boardOffSet;
                cout << "PanOffset = LEFT" << endl;
                break;
            case RIGHT:
                end += (cropFrameWidth / 2) - boardOffSet;
                cout << "PanOffset = RIGHT" << endl;
                break;
            case CENTER:
                break;
        }


        int currentPos = start;

        int pixels = (int) abs(end - start);

        double sampleSize = 2 * CV_PI / numFrames;
        double normal = pixels / numFrames;

        for (int i = 0; i < numFrames; i++)
        {

            long double outP = smooth(i * sampleSize) * normal;

            //Needed to move rectangle left/right depending on the direction the lecturer is moving in
            if (right)
            {
                currentPos += outP;
            } else
            {
                currentPos -= outP;
            }

            //Creates a cropping rectangle
            setPan(currentPos, currentPan);


            croppingRectangles.push_back(currentPan);

        }
    } else
    {

        //Dont pan. just push back current crop
        //TODO - Can add re-centering feature here.
        for (int i = 0; i < numFrames; i++)
        {
            croppingRectangles.push_back(currentPan);
        }
    }
}

//Essentially the main method for this class
void PanLogic::doPan(std::vector<PresenterMotion::Movement> &motionLines, std::vector<cv::Rect> &croppingRectangles)
{

    int sP;

    //Loop over all motion lines
    for (int i = 0; i < motionLines.size(); i++)
    {
        //TODO - For each line determine if needed to pan!!! Currently just following lecturer all the time

        PresenterMotion::Movement m = motionLines.at(i);
        if (!m.isDropData)
        {
            //This is an actual motion line
            //TODO CENTER is the board offset
            if (i > 0)
            {
                sP = getPan(currentPan);

            } else
            {
                sP = m.start.x;
            }

            //If boards used in this motion line
            if (motionLines.at(i).boardUsed)
            {
                //True should be board used
                if (!motionLines.at(i).rightBoardUsed)
                {
                    cout << "Left Board Usage Activated in panning Logic" << endl;
                    smoothMove(sP, m.end.x, m.numFrames, m.right, LEFT, croppingRectangles);

                } else
                {
                    cout << "Right Board Usage Activated in panning Logic" << endl;
                    smoothMove(sP, m.end.x, m.numFrames, m.right, RIGHT, croppingRectangles);
                }

            } else
            {
                cout << "Center or No Board Usage Activated in panning Logic" << endl;
                smoothMove(sP, m.end.x, m.numFrames, m.right, CENTER, croppingRectangles);
            }

        } else
        {

            //Todo - check if noise fits into pan region + Align correctly
            //This is dropped frames (noise)
            //Get last rectangle and duplicate because we want crop region to remain still where the lecturer doesnt move significantly
            //Rect r = croppingRectangles.at(croppingRectangles.size() - 1);

            if (m.numFrames > 50)
            {
                int spos = getPan(currentPan);
                int epos = m.start.x + abs(m.end.x - m.start.x);
                bool r = false;
                if(spos<epos){
                    r=true;
                }

                smoothMove(spos,epos, m.numFrames, r, CENTER,
                           croppingRectangles);
            } else
            {
                int speed = motionLines.at(i).numFrames; //Number of frames to reposition over

                /*

                if (motionLines.at(i).boardUsed)
                {
                    //True should be board used
                    if (!motionLines.at(i).rightBoardUsed)
                    {
                        cout << "Reposition LEFT" << endl;
                        rePosition(LEFT, speed, m, croppingRectangles);

                    } else
                    {
                        cout << "Reposition RIGHT" << endl;
                        rePosition(RIGHT, speed, m, croppingRectangles);
                    }

                } else
                {
                    cout << "Reposition CENTER" << endl;
                    rePosition(CENTER, speed, m, croppingRectangles);
                }
    */

                int rem = speed;
                for (int j = 0; j < rem; j++)
                {
                    croppingRectangles.push_back(currentPan);
                }
            }

        }
    }
}

void PanLogic::rePosition(Position moveToPosition, int numFrames, PresenterMotion::Movement &m,
                          std::vector<cv::Rect> &croppingRectangles)
{

    int s = getPan(currentPan);
    int e;
    bool right;

    switch (moveToPosition)
    {
        case LEFT:
            if (m.start.x > m.end.x)
            {
                e = m.start.x;
            } else
            {
                e = m.end.x;
            }
            break;

        case RIGHT:
            if (m.start.x < m.end.x)
            {
                e = m.start.x;
            } else
            {
                e = m.end.x;
            }
            break;
        case CENTER:
            if (m.start.x < m.end.x)
            {
                e = m.start.x + m.length() / 2;
            } else
            {
                e = m.start.x - m.length() / 2;
            }
            break;
    }

    right = s > e;

    smoothMove(s, e, numFrames, right, moveToPosition, croppingRectangles);

}

//Method to initialise the dimensions of the panning class
void PanLogic::initialise(cv::Size inputFrameSize, cv::Size cropSize, int yPanLevel)
{

    cropFrameWidth = cropSize.width;
    cropFrameHeight = cropSize.height;
    inputFrameWidth = inputFrameSize.width;
    inputFrameHeight = inputFrameSize.height;

    yLevelOfPanWindow = yPanLevel;

    boardOffSet = (int) (0.1 * cropFrameWidth);

}

bool PanLogic::checkIfPanRequired(int start, int end)
{

    //Add some buffer padding
    return start >= currentPan.tl().x + 1.5 * boardOffSet && end >= currentPan.tl().x + 1.5 * boardOffSet &&
           start <= currentPan.br().x - 1.5 * boardOffSet && end <= currentPan.br().x - 1.5 * boardOffSet;

}

void PanLogic::setPan(int currentPosX, Rect &crop)
{
    Rect tmp = Rect(currentPosX - (cropFrameWidth / 2), yLevelOfPanWindow, cropFrameWidth, cropFrameHeight);
    if (!inBounds(tmp))
    {
        if (initialCrop)
        {
            if (currentPosX > cropFrameWidth)
            {
                tmp = Rect(inputFrameWidth - cropFrameWidth, yLevelOfPanWindow, cropFrameWidth, cropFrameHeight);
            } else
            {
                tmp = Rect(0, yLevelOfPanWindow, cropFrameWidth, cropFrameHeight);
            }

            initialCrop = false;
            currentPan = tmp;
        }
        crop = currentPan;
    } else
    {
        crop = tmp;
    }
}

bool PanLogic::inBounds(int x)
{

    return x >= 0 && (x - cropFrameWidth / 2 + cropFrameWidth) <= inputFrameWidth;
}

bool PanLogic::inBounds(cv::Rect &crop)
{
    return crop.tl().x >= 0 && crop.tl().x + cropFrameWidth <= inputFrameWidth;
}

int PanLogic::getPan(cv::Rect &crop)
{
    return crop.x + (cropFrameWidth / 2);
}


