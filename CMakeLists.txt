cmake_minimum_required(VERSION 3.5)
project(Track4K)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
#set(CMAKE_CXX_STANDARD 11)

set(OpenCV_DIR "C:\\opencv\\build\\install")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE_FILES FileReader.cpp FileReader.h MetaFrame.cpp MetaFrame.h "Board Segmentation/source/Track4KPreProcess.cpp" CodeTimer.cpp CodeTimer.h "Board Segmentation/source/IlluminationCorrection.cpp" "Board Segmentation/headers/IlluminationCorrection.h" "Board Segmentation/source/MotionDetection.cpp" "Board Segmentation/headers/MotionDetection.h" Panning/source/VirtualCinematographer.cpp Panning/headers/VirtualCinematographer.h Panning/source/VideoOutput.cpp Panning/headers/VideoOutput.h Panning/source/FrameCropper.cpp Panning/headers/FrameCropper.h "Board Segmentation/headers/Track4KPreProcess.h" "Board Segmentation/source/BoardDetection.cpp" "Board Segmentation/headers/BoardDetection.h" Panning/source/PresenterMotion.cpp Panning/headers/PresenterMotion.h Panning/source/PanLogic.cpp Panning/headers/PanLogic.h mainDriver.cpp PersistentData.h PersistentData.cpp PointPlotter.cpp PointPlotter.h Tracking/headers/RecognitionDriver.h Tracking/source/RecognitionDriver.cpp
        Tracking/headers/MovementDetection.h Tracking/source/MovementDetection.cpp Tracking/source/Ghost.cpp Tracking/headers/Ghost.h)
add_executable(Track4K ${SOURCE_FILES})


set(OpenCV_LIBS opencv_calib3d opencv_core opencv_features2d opencv_flann opencv_highgui opencv_imgcodecs opencv_imgproc opencv_ml opencv_objdetect opencv_photo opencv_shape opencv_stitching opencv_superres opencv_video opencv_videoio opencv_videostab opencv_xfeatures2d)

target_link_libraries(Track4K ${OpenCV_LIBS})

