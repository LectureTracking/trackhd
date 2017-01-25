//
// Created by Charles on 2016/10/25.
//

#ifndef TRACK4K_POINTPLOTTER_H
#define TRACK4K_POINTPLOTTER_H
#include "panning/VideoOutput.h"
#include "panning/FrameCropper.h"
#include "opencv2/opencv.hpp"
#include "panning/PresenterMotion.h"
#include "panning/PanLogic.h"
#include <fstream>
#include "FileReader.h"

class PointPlotter
{
public:
    void plotPoints(PersistentData &pD);
};


#endif //TRACK4K_POINTPLOTTER_H
