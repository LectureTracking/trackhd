/**
 *  Copyright 2016 Charles Fitzhenry / Mohamed Tanweer Khatieb / Maximilian Hahn
 *  Licensed under the Educational Community License, Version 2.0
 *  (the "License"); you may not use this file except in compliance
 *  with the License. You may obtain a copy of the License at
 *
 *  http://www.osedu.org/licenses/ECL-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS"
 *  BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 *  or implied. See the License for the specific language governing
 *  permissions and limitations under the License.
 *
 */

//
// Created by Mohamed Tanweer Khatieb on 2016/09/16.
//

#include "PresenterMotion.h"

using namespace std;
using namespace cv;


/**
 * Given a vector of points (Lecturer positions), create line segments each time the direction of motion changes.
 * @param lecturerPositions
 * @param skipFrames
 */
void PresenterMotion::generateMotionLines(std::vector<Point> lecturerPositions, int skipFrames)
{
    bool right;
    right = lecturerPositions.at(0).x < lecturerPositions.at(1).x;
    int counter = skipFrames;
    Point endPoint;
    Point startPoint = lecturerPositions.at(0);

    Movement movement;
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
            counter += skipFrames;
        }
        else if (prev.x > current.x && !right)
        {
            counter += skipFrames;
        }
        else
        {
            endPoint = prev;
            movement.start = startPoint;
            movement.end = endPoint;
            movement.numFrames = counter;
            movement.right = right;
            motion.push_back(movement);
            right = !right;
            startPoint = endPoint;
            counter = skipFrames;
        }
    }
    endPoint = prev;
    movement.start = startPoint;
    movement.end = endPoint;
    movement.numFrames = counter;
    movement.right = right;
    motion.push_back(movement);
}

/**
 * Reduces jittery movements by fusing short lines
 * @param ignoreThresh
 * @param yFix
 */
void PresenterMotion::cullMotion(int ignoreThresh, int yFix)
{

    vector<Movement> tempMovements;
    int dropCounter = 0;

    int minX = INT_MAX;
    int maxX = INT_MIN;

    for (Movement movement:motion)
    {

        if (movement.length() > ignoreThresh && (movement.length() / movement.numFrames) < 25)
        {

            if (dropCounter > 0)
            {
                Movement movement1;
                movement1.numFrames = dropCounter;
                movement1.isDropData = true;
                movement1.start = Point(minX, yFix);
                movement1.end = Point(maxX, yFix);
                tempMovements.push_back(movement1);
                dropCounter = 0;
                minX = INT_MAX;
                maxX = INT_MIN;
            }
            tempMovements.push_back(movement);
        }
        else
        {

            dropCounter += movement.numFrames;

            if (movement.start.x < minX)
            {
                minX = movement.start.x;
            }

            if (movement.end.x > maxX)
            {
                maxX = movement.end.x;
            }

            if (movement.end.x < minX)
            {
                minX = movement.end.x;
            }

            if (movement.start.x > maxX)
            {
                maxX = movement.start.x;
            }
        }
    }

    if (dropCounter > 0)
    {
        Movement movement;
        movement.numFrames = dropCounter;
        movement.isDropData = true;
        movement.start = Point(minX, yFix);
        movement.end = Point(maxX, yFix);
        tempMovements.push_back(movement);
    }

    motion.clear();
    motion = tempMovements;
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
            //For motion going left
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


/**
 * Stitches together smaller pan operations going in the same direction (interrupted by noise).
 * @param ignoreThresh
 */
void PresenterMotion::repairCulling(int ignoreThresh)
{

    std::vector<Movement> temp;
    int dropFramesCount = 0;
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


        if (current.isDropData)
        {
            if (current.numFrames < ignoreThresh)
            {
                dropFramesCount += current.numFrames;
            }
            else
            {
                if (first)
                {
                    temp.push_back(current);
                }
                else
                {
                    m_new.numFrames += dropFramesCount;
                    dropFramesCount = 0;
                    temp.push_back(m_new);
                    temp.push_back(current);
                    first = true;
                }
            }
        }
        else
        {
            if (first)
            {
                m_new.isDropData = false;
                m_new.start = current.start;
                m_new.end = current.end;
                m_new.right = current.right;
                m_new.numFrames += current.numFrames;
                first = false;
            }
            else if (!first && (current.right == m_new.right))
            {
                m_new.isDropData = false;
                m_new.end = current.end;
                m_new.right = current.right;
                m_new.numFrames += current.numFrames;
                first = false;
            }
            else if (!first && (current.right != m_new.right))
            {
                temp.push_back(m_new);
                m_new.isDropData = false;
                m_new.start = current.start;
                m_new.end = current.end;
                m_new.right = current.right;
                m_new.numFrames = 0;
                m_new.numFrames += dropFramesCount;
                dropFramesCount = 0;
                m_new.numFrames += current.numFrames;
            }
        }
    }

    if (temp.at(temp.size() - 1).start != m_new.start && temp.at(temp.size() - 1).end != m_new.end)
    {
        if (dropFramesCount > 0)
        {
            m_new.numFrames += dropFramesCount;
            temp.push_back(m_new);
        }
        else
        {
            temp.push_back(m_new);
        }
    }
    motion.clear();
    motion = temp;
}

/**
 * Creates a motion graph where each line represents a pan operation. Blue is left, red is right, white is noise and the length is the distance on the x-plane.
 * @param img
 */
void PresenterMotion::generateMotionImage(Mat &img)
{

    int yOff = 20;
    int addOn = 0;
    for (Movement m:motion)
    {
        Scalar s;
        if (m.isDropData)
        {
            s = Scalar(255, 255, 255);
            DrawLine(img, Point(m.start.x, yOff + addOn), Point(m.end.x, yOff + addOn), s);
            addOn += yOff;
        }
        else
        {
            if (m.right)
            {
                s = Scalar(0, 0, 255);
            }
            else
            {
                s = Scalar(255, 170, 0);
            }
            DrawLine(img, Point(m.start.x, yOff + addOn), Point(m.end.x, yOff + addOn), s);
            addOn += yOff;
        }
    }
}

/**
 * Aligns jagged edges between pans (i.e The start point of the next pan is the end of the current pan).
 */
void PresenterMotion::relinkMotion()
{
    int continueFrom = 0;
    bool last_x = false;
    for (int i = 0; i < motion.size(); i++)
    {
        Movement m = motion.at(i);
        if (!m.isDropData)
        {
            if (!last_x)
            {
                continueFrom = m.end.x;
            }
            if (i > 0 && last_x)
            {
                motion.at(i).start.x = continueFrom;
                last_x = false;
            }
        }
        else
        {
            last_x = true;
            continueFrom = m.end.x;
        }
    }
}

/**
 * Returns by reference through parameter given
 * @param outMotionVec
 */
void PresenterMotion::getMotionLines(vector<PresenterMotion::Movement> &outMotionVec)
{
    outMotionVec = motion;
}

/**
 * Helper method used by the generateMotionImage() method to draw the actual line segments onto an image
 * @param img
 * @param start
 * @param end
 * @param s
 */
void PresenterMotion::DrawLine(Mat &img, Point start, Point end, Scalar s)
{

    int thickness = 10;
    int lineType = 8;
    line(img, start, end, s, thickness, lineType);
}

/**
 * Labels a motion line if (and where) a board was used.
 * @param persistentData
 */
void PresenterMotion::attatchBoardUsage(PersistentData &persistentData)
{
    int rangeStart = 0;
    int rangeEnd = 0;

    int checkpoint = 0;

    for (int i = 0; i < motion.size(); i++)
    {
        rangeEnd = rangeStart + motion.at(i).numFrames;

        for (int j = checkpoint; j < persistentData.metaFrameVector.size(); j++)
        {
            if (j * persistentData.boardDetectionSkipFrames >= rangeStart && j * persistentData.boardDetectionSkipFrames < rangeEnd)
            {
                if (persistentData.metaFrameVector.at(j).leftProjector || persistentData.metaFrameVector.at(j).leftBoard)
                {
                    motion.at(i).boardUsed = true;
                    motion.at(i).rightBoardUsed = false;
                }
                else if (persistentData.metaFrameVector.at(j).rightProjector || persistentData.metaFrameVector.at(j).rightBoard)
                {
                    motion.at(i).boardUsed = true;
                    motion.at(i).rightBoardUsed = true;
                }

                checkpoint = j;
            }
        }
        rangeStart = rangeEnd;
    }
}