# TRACK4K
Track4K is an open source C++ project that takes a High Definition video of a lecture recording and then produces a smaller cropped output video, which frames the lecturer. This is done using image processing and computer vision algorithms to track the lecturer and uses the lecturer position information to pan the virtual camera.

## Getting Started
These instructions will help get the program and all its dependencies set up on your machine.

### Prerequisites
These instructions are written with the assumption that the project will be installed on a Linux-based system (preferably a Debian version). To be able to run this project, you will need to first install the following dependencies:
* OpenCV 3
* OpenCV Extra Modules
* C++ Libraries
* CMake
* git

### Installation

#### Downloading and Installing base dependencies
The first on the install list (and most important) is CMake, followed by git and C++.
The following terminal command will get and install the necessary requirements

```
$ sudo apt-get install cmake git build-essential libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
```

#### Downloading and Installing the OpenCV libraries
Next, we need to download and install the OpenCV libraries.
The needed OpenCV library comes in two components. First download the core OpenCV library. Choose any directory as your directory to download these files into.
Clone OpenCV from Git as follows:
```
$ cd <your_chosen_working_directory>
$ git clone https://github.com/opencv/opencv

```
Next, repeat the process for the Extra modules. Remain in the same working directory and execute the following terminal command:
```
$ git clone https://github.com/opencv/opencv_contrib
```
You should now have two folders in your working directory.
The next step is to build OpenCV.

#### Building the OpenCV library

Firstly change into the OpenCV directory (installation destination). There should be two folders, one is the main OpenCV library and the other contains the Extra modules.

```
$ cd <your_chosen_working_directory>
```
Inside the main OpenCV folder, change directory into the build folder and remove all files, since it will require rebuilding. To rebuild OpenCV run the following command from within the build folder:

```
$ cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
```
This step will generate a MakeFile. Once complete perform the following command to run make faster (the number after the j-flag is the number of processors the job will use).

```
$ make -j8
```
Remain in the build folder and run the following cmake command to make the extra modules.
The path decribed below is an example. Fill in the directory path on your machine which points to the OpenCV Extra modules folder.

```
cmake -DOPENCV_EXTRA_MODULES_PATH=<OpenCV Extra Modules Folder Path>/modules ../
```
Next, we need to make these files:

```
$ make -j8
```

Finally, we need to install these modules. Run the following command:
```
$ sudo make install
```

## Running the program

Run the program as follows:
```
$ ./Track4K <inputVideoFileName.extension> <outputVideoFileName.extension> <outputFrameWidth> <outputFrameHeight> [FOURCC Codec code]
```
The FOURCC codec parameter is optional (default CODEC is X264).
To see all available codecs, visit [FOURCC](https://www.fourcc.org/codecs.php)

## Built With

* [OpenCV](http://www.opencv.org) - The computer vision library of choice

## License

  Copyright 2016 Charles Fitzhenry / Mohamed Tanweer Khatieb / Maximilian Hahn
  Licensed under the Educational Community License, Version 2.0
  (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

  http://www.osedu.org/licenses/ECL-2.0

  Unless required by applicable law or agreed to in writing,
  software distributed under the License is distributed on an "AS IS"
  BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
  or implied. See the License for the specific language governing
  permissions and limitations under the License.
