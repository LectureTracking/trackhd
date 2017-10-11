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
// Created by Maximilian Hahn on 2016/08/09.
//

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "ImageRecognition.h"

#include <iostream>
#include <stdio.h>


using namespace std;
using namespace cv;

bool ImageRecognition::loadImage(string imageName){

    //load image in colour and display
    Mat image;
    image = imread(imageName, CV_LOAD_IMAGE_GRAYSCALE);

    //if image failed to load
    if(image.empty()){
        std::cout << "\nImage load " << imageName << " failed!" << std::endl;
        return false;
    }

    //namedWindow("window1", 1);
    //imshow("window1", image);

    //load face cascade (.xml file)
    CascadeClassifier face_cascade;
    face_cascade.load("C:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_default.xml");

    //Detect face
    vector<Rect> faces;
    //
    face_cascade.detectMultiScale(image,
                                  faces, //vector of rectangles on detected objects
                                  1.1, //how much image size is reduced at each scale
                                  5, //how many min neighbours to have to retain
                                  0 | CV_HAAR_SCALE_IMAGE, //search type flag
                                  Size(
                                          20, //min scale of object
                                          25 //max scale of object (max = min = unbounded)
                                  ));

    //Draw circles on the detected faces
    for(int i = 0; i < faces.size(); i++){
        Point center(faces[i].x + faces[i].width * 0.5, faces[i].y + faces[i].height * 0.5);
        ellipse(image, center, Size(faces[i].width * 0.5, faces[i].height * 0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
    }

    imshow("Detected Face", image);

    waitKey(0);

    return true;
}