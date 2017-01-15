//
// Created by Tanweer Khatieb on 2016/09/16.
//

#include "../headers/PresenterMotion.h"
#include "../../PersistentData.h"


using namespace std;
using namespace cv;


//Given a vector of points (Lecturer positions), create line segments each time the direction of motion changes.
// This includes very small line segments and a separate method deals with culling of these
void PresenterMotion::generateMotionLines(std::vector<Point> lecturerPositions, int skipFrames)
{

    //TODO - Simulate points that are read in from the text file


    //Set initial direction
    bool right;
    right = lecturerPositions.at(0).x < lecturerPositions.at(1).x;

    int counter = skipFrames; //The counter tracks the number of frames that have passed
    Point endPoint;

    //TODO - Initialize start point correctly
    //This is the very first position of the lecturer.
    Point startPoint = lecturerPositions.at(0);

    Movement m;
    Point prev;
    Point current;
/*
 * @counter stores the number of frames processed
 */
    for (int i = 1; i < lecturerPositions.size(); i++)
    {
        prev = lecturerPositions.at(i - 1);
        current = lecturerPositions.at(i);


        if (prev.x < current.x && right)
        {
            //Going right and there is no change
            //Add the frame to the line vector
            counter+= skipFrames;

        } //else if(prev.x>current.x && right){
        else if (prev.x > current.x && !right)
        {
            //Goin left and there is no change
            //Add frame to the line vector
            counter+=skipFrames;

        } else
        {

            //counter++;
            //TODO Cannot be current because current has already changed the direction ---Has to be previous--- ***Has been fixed
            endPoint = prev;


            m.start = startPoint;
            m.end = endPoint;
            m.numFrames = counter;
            m.right = right;

            motion.push_back(m);

            right = !right;
            startPoint = endPoint;
            counter =skipFrames;

        }
    }

    endPoint = prev;


    m.start = startPoint;
    m.end = endPoint;
    m.numFrames = counter;
    m.right = right;

    motion.push_back(m);
}

void PresenterMotion::checkLogic()
{

    //Just to debug
    int sm = 0;
    for (int i = 0; i < motion.size(); i++)
    {
        sm += motion.at(i).numFrames;
    }
    cout << "[3] Number of frames as stored by motion Lines: " << sm << endl;

}

//Used to remove all line segments that are shorter than the given threshold value
void PresenterMotion::cullMotion(int ignoreThresh, int yFix)
{

    vector<Movement> temp;
    int dropCounter = 0;
    int avgX = 0;
    int avgY = 0;

    int minX = INT_MAX;
    int maxX = INT_MIN;

    for (Movement m:motion)
    {

        if (m.length() > ignoreThresh && (m.length()/m.numFrames)<25)
        {

            if (dropCounter > 0)
            {
                Movement mv;
                mv.numFrames = dropCounter;
                mv.isDropData = true;

                //Set points
                mv.start = Point(minX, yFix);
                mv.end = Point(maxX, yFix);
                temp.push_back(mv);
                dropCounter = 0;
                minX = INT_MAX;
                maxX = INT_MIN;
            }
            temp.push_back(m);
        } else
        {

            dropCounter += m.numFrames;
            //dropCounter++;
            if (m.start.x < minX)
            {
                minX = m.start.x;
            }

            if (m.end.x > maxX)
            {
                maxX = m.end.x;
            }

            if (m.end.x < minX)
            {
                minX = m.end.x;
            }

            if (m.start.x > maxX)
            {
                maxX = m.start.x;
            }
        }
    }

    if (dropCounter > 0)
    {
        Movement mv;
        mv.numFrames = dropCounter;
        mv.isDropData = true;

        //Set points
        mv.start = Point(minX, yFix);
        mv.end = Point(maxX, yFix);
        temp.push_back(mv);
        dropCounter = 0;
        minX = INT_MAX;
        maxX = INT_MIN;
    }

    motion.clear();
    motion = temp;
}

/*void PresenterMotion::repairCulling(int ignoreThresh)
{

    std::vector<Movement> temp;
    Movement m_new;
    for (int i = 0; i < motion.size(); i++)
    {
        if (i > motion.size() - 1)
        {
            break;
        }

        //For motion going right
        if (!motion[i].isDropData && motion[i].right)
        {
            if (motion[i + 1].isDropData &&
                (!motion[i + 2].isDropData && motion[i + 2].right && motion[i + 1].numFrames <= ignoreThresh))
            {
                m_new.start = motion[i].start;
                m_new.end = motion[i + 2].end;
                m_new.numFrames = motion[i].numFrames + motion[i + 1].numFrames + motion[i + 2].numFrames;
                m_new.isDropData = false;
                m_new.right = true; //TODO
                temp.push_back(m_new);
                i += 2;
                //Case 2
            } else if (motion[i + 1].isDropData &&
                       (!motion[i + 2].isDropData && !motion[i + 2].right))
            {
                temp.push_back(motion[i]);
                temp.push_back(motion[i + 1]);
                temp.push_back(motion[i + 2]);
                i += 2;

            } else if (!motion[i + 1].isDropData && !motion[i + 1].right)
            {

                temp.push_back(motion[i]);
                temp.push_back(motion[i + 1]);
                i++;

            } else if (motion[i + 1].isDropData && motion[i + 1].numFrames > ignoreThresh)
            {
                temp.push_back(motion[i]);
                temp.push_back(motion[i + 1]);
                temp.push_back(motion[i + 2]);
                i += 2;
            }
            //For motiong going left
        } else if (!motion[i].isDropData && !motion[i].right)
        {
            if (motion[i + 1].isDropData &&
                (!motion[i + 2].isDropData && !motion[i + 2].right && motion[i + 1].numFrames < ignoreThresh))
            {
                m_new.start = motion[i].start;
                m_new.end = motion[i + 2].end;
                m_new.numFrames = motion[i].numFrames + motion[i + 1].numFrames + motion[i + 2].numFrames;
                m_new.isDropData = false;
                m_new.right = false; //TODO
                temp.push_back(m_new);
                i += 2;
            } else if (motion[i + 1].isDropData &&
                       (!motion[i + 2].isDropData && motion[i + 2].right))
            {
                temp.push_back(motion[i]);
                temp.push_back(motion[i + 1]);
                temp.push_back(motion[i + 2]);
                i += 2;

            } else if (!motion[i + 1].isDropData && motion[i + 1].right)
            {

                temp.push_back(motion[i]);
                temp.push_back(motion[i + 1]);
                i++;

            } else if (motion[i + 1].isDropData && motion[i + 1].numFrames > ignoreThresh)
            {
                temp.push_back(motion[i]);
                temp.push_back(motion[i + 1]);
                temp.push_back(motion[i + 2]);
                i += 2;
            }

        }
            //For non-motion (Noise)
        else
        {
            temp.push_back(motion[i]);
        }
    }
    motion.clear();
    motion = temp;
}*/

void PresenterMotion::repairCulling(int ignoreThresh)
{

    std::vector<Movement> temp;
    int droppFramesCount = 0;
    Movement m_new;
    m_new.numFrames = 0;
    Movement current;
    Movement next;
    bool first = true;
    bool right = false;

    for (int i = 0; i < motion.size(); i++)
    {
        current = motion.at(i);

        if (i > motion.size() - 1)
        {
            break;
        }


        if(current.isDropData){
            //Noise
            if(current.numFrames<ignoreThresh){
                droppFramesCount+=current.numFrames;
            } else{
                if(first){
                    temp.push_back(current);
                } else{
                    m_new.numFrames+=droppFramesCount;
                    droppFramesCount=0;
                    temp.push_back(m_new);
                    temp.push_back(current);
                    first = true;
                }

            }

        } else{
            //Actual motion
            if(first){
                m_new.isDropData = false;
                m_new.start = current.start;
                m_new.end = current.end;
                m_new.right = current.right;
                m_new.numFrames+=current.numFrames;
                first = false;
            } else if(!first && (current.right==m_new.right)){
                m_new.isDropData = false;
                m_new.end = current.end;
                m_new.right = current.right;
                m_new.numFrames+=current.numFrames;
                first = false;
            } else if(!first && (current.right!=m_new.right)){
                //Change in direction
                temp.push_back(m_new);





                //Set up new temp
                m_new.isDropData = false;
                m_new.start = current.start;
                m_new.end = current.end;
                m_new.right = current.right;
                m_new.numFrames = 0;
                m_new.numFrames+=droppFramesCount;
                droppFramesCount = 0;
                m_new.numFrames+=current.numFrames;
            }

        }


    }

    if(temp.at(temp.size()-1).start != m_new.start && temp.at(temp.size()-1).end != m_new.end){
        if(droppFramesCount>0){
            m_new.numFrames+=droppFramesCount;
            temp.push_back(m_new);

        } else{
            temp.push_back(m_new);
        }
    }


    motion.clear();
    motion = temp;
}


//Takes an empty image in and populates it with the lines depicting the motion graphically
void PresenterMotion::generateMotionImage(Mat &img)
{

    int yOff = 20;
    int addOn = 0; //Just to space lines on diagram
    for (Movement m:motion)
    {
        Scalar s;
        if (m.isDropData)
        {

            s = Scalar(255, 255, 255);
            DrawLine(img, Point(m.start.x, yOff + addOn), Point(m.end.x, yOff + addOn), s);
            addOn += yOff;

        } else
        {

            if (m.right)
            {
                s = Scalar(0, 0, 255);
            } else
            {
                s = Scalar(255, 170, 0);
            }

            DrawLine(img, Point(m.start.x, yOff + addOn), Point(m.end.x, yOff + addOn), s);
            addOn += yOff;
        }
    }
}

void PresenterMotion::relinkMotion()
{

    int continueFrom = 0;
    bool last_x = false;

    for (int i = 0; i < motion.size(); i++)
    {

        Movement m = motion.at(i);

        if (!m.isDropData)
        {
            //This is an actual motion line

            if (!last_x)
            {
                continueFrom = m.end.x;
            }


            if (i > 0 && last_x)
            {

                motion.at(i).start.x = continueFrom;
                last_x = false;

            }

        } else
        {

            last_x = true;
            continueFrom = m.end.x;


        }

    }
}

//Basically a get method - Returns by reference through parameter given
void PresenterMotion::getMotionLines(vector<PresenterMotion::Movement> &outMotionVec)
{

    outMotionVec = motion;
}

//Helper method used by the generateMotionImage() method to draw the actual line segments onto an image
void PresenterMotion::DrawLine(Mat &img, Point start, Point end, Scalar s)
{

    int thickness = 10;
    int lineType = 8;
    line(img,
         start,
         end,
         s,
         thickness,
         lineType);
}

//Just a method used for debugging - For each line segment, prints out details about it
void PresenterMotion::printLines()
{

    for (int i = 0; i < motion.size(); i++)
    {
        if (motion.at(i).isDropData)
        {
            cout << "This is dropped data. AVG_Start =" << motion.at(i).start.x << " AVG_End=" << motion.at(i).end.x
                 << " FrameCount="
                 << motion.at(i).numFrames << endl;
        } else
        {
            cout << "Start=" << motion.at(i).start.x << " End=" << motion.at(i).end.x << " FrameCount="
                 << motion.at(i).numFrames << endl;
        }

    }
}


void PresenterMotion::attatchBoardUsage(PersistentData &pD)
{
    int rangeS = 0;
    int rangeE = 0;

    int checkpoint = 0;

    for (int i = 0; i < motion.size(); i++)
    {
        int rangeE = rangeS + motion.at(i).numFrames;

        for (int j = checkpoint; j < pD.metaFrameVector.size(); j++)
        {
            if (j * pD.boardDetectionSkipFrames>=rangeS && j * pD.boardDetectionSkipFrames<rangeE) //Finds corresponding motion line to attach board usage to
            {
                if (pD.metaFrameVector.at(j).leftProjector || pD.metaFrameVector.at(j).leftBoard)
                {
                    motion.at(i).boardUsed = true;
                    motion.at(i).rightBoardUsed = false;
                } else if (pD.metaFrameVector.at(j).rightProjector || pD.metaFrameVector.at(j).rightBoard)
                {
                    motion.at(i).boardUsed = true;
                    motion.at(i).rightBoardUsed = true;
                }

                checkpoint = j;
            }
        }
        rangeS = rangeE;
    }
}