# TRACK4K
Track4K is an open source C++ project that takes a High Definition video of a lecture recording and then produces a smaller cropped output video, which frames the lecturer. This is done using image processing and computer vision algorithms to track the lecturer and uses the lecturer position information to pan the virtual camera.

## Getting Started

> If you want to use right away go to [this link](/utils/Ansible_Playbook) to install Track4K automatically.

These instructions will help get the program and all its dependencies set up on your machine.

> Please take note that this installation guide It was made for use under Ubuntu 16.04, some changes may apply for other distributions or Ubuntu variations.

> All the commands are run as normal user unless if its written as super user "\#"





### Prerequisites
These instructions are written with the assumption that the project will be installed on a Linux-based system (preferably a Debian version). **Track4K has been tested on Ubuntu 16.04**

 To be able to run this project, you will need to first install the following dependencies:

* ffmpeg (3.4 or newer)
* OpenCV 3 (3.2.0 or future releases)
* OpenCV Extra Modules (latest version on repository)
* C++ Libraries (6.3 or future releases)
* CMake (3.8.0 or future releases)
* git (2.10.2 or future releases)



## Installation of the requirements


First go to any folder to work with the files that will be downloaded and installed. Next, install the dependencies in the order are written

### Basic libraries

```
sudo apt update
sudo apt install git build-essential libgtk2.0-dev pkg-config
```

### CMAKE

```
wget https://cmake.org/files/v3.11/cmake-3.11.4-Linux-x86_64.sh
sudo mkdir /opt/cmake$ sudo sh cmake-3.11.4-Linux-x86_64.sh --prefix=/opt/cmake --skip-license
sudo update-alternatives --install /usr/bin/cmake cmake /opt/cmake/bin/cmake 1 --force</code></pre>
```

###  C and C++

The C and C++ libraries from Ubuntu's official repositories are older than the libraries required. It's needed to have the C and C++ from version 6.3 or newer.

#### Install the repository with the updated versions of C and C++
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
```

#### C Libraries installation
```
sudo apt install gcc-7
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 60 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-7 --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-7 --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-7
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-5 --slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-5 --slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-5
```

#### C++ Libraries installation
```
sudo apt install g++-7
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 60
```

### Install FFMPEG 3

#### FMPEG Main appllication
```
sudo add-apt-repository ppa:jonathonf/ffmpeg-3
sudo apt update
sudo apt install ffmpeg
```

#### FFMPEG Development libraries
```
sudo apt install libavcodec-dev libavformat-dev libavfilter-dev
sudo apt install libx265-dev libx264-dev libvpx-dev libbz2-dev libvdpau-dev libva-dev liblzma-dev
```

## Installation of Track4K

#### Clone the repositories:
```
git clone https://github.com/opencv/opencv
git clone https://github.com/opencv/opencv_contrib
git clone https://github.com/cilt-uct/trackhd.git
```

#### Install OpenCV

> **Note: Track 4K works with version 3.4 of OpenCV, prerelease of V4.0 makes compilation errors.**

In the OpenCV directory, Change to the 3.4 branch, next, build the program
```
cd opencv
git checkout --track remotes/origin/3.4
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
```

Once complete perform the following command to run make faster (the number after the j-flag is the number of processors the job will use). If you are not sure how many processors the machine has use the following instruction to find out:
```
cat /proc/cpuinfo | grep processor | wc -l
```

Use the result from this in the j-flag

```
make -j`processor_count`
```

Remain in the build folder and run the following cmake command to make the extra modules. The path described below is an example. Fill in the directory path on your machine which points to the OpenCV Extra modules folder.

```
cmake -DOPENCV_EXTRA_MODULES_PATH=<Repositories folder path>/opencv_contrib/modules ../
```

After that compile and install the files:

```
make -j`processor_count`
sudo make install
```

#### Building Track4K

##### Automatic Method

There is a shell script in the trackhd folder called `install_track4k.sh` which can be used to install track4k automatically.
To use this script run the following command:

```
sudo ./install_track4k.sh
```

This will run all the steps listed in the manual method mentioned below.

##### Manual Method

This method is for the case where the automatic method does not work. It does everything the shell script does manually.

The trackhd directory should have 2 main folders inside it: source and build. The source folder contains all the header and source files while the build file contains all object files and executables.
The first step is to navigate into the build folder. Once inside run delete all files (if any) and then type the following command in terminal:

```
cmake ../source
```

Now it is possible to run the build instruction:

```
make -j`number_of_processors`
```
You can now install the project to /usr/local/bin/ by running the following command:

```
sudo make install
```

Then build cropvid:

```
cd cropvid
./build.sh
cp cropvid /usr/local/bin/
```

#### Running Track4K

Track4K runs in two parts: track4k analyses a video file and produces a cropping data file in text format. cropvid crops the
video file according to the cropping information in the data file, using ffmpeg libraries.

```
track4k <inputFileName> <outputFileName> <output-width> <output-height>
cropvid <input file> <output file> <cropping file>
```

**Example:**

```
track4k presenter.mkv presenter-crop.txt 1920 1080
cropvid presenter.mkv tracked.mkv presenter-crop.txt
```

Track4K can also output the cropping information in JSON format, when the output filename has a `.json` extension:

Example:

```
track4k presenter.mkv presenter-crop.json 1920 1080
```

The JSON format includes a timestamp as well as a frame number. The timestamp is only guaranteed to be accurate when
the source video has been recorded with a fixed frame rate. It may be incorrect for variable frame rate source videos.

### Memory Requirements
The program reads a maximum of 29 frames into memory at a time. So a minimum of 4GB RAM should be sufficient.

## Built With

[OpenCV](http://www.opencv.org) - The computer vision library of choice
[FFFMPEG](https://www.ffmpeg.org) - A complete, cross-platform solution to record, convert and stream audio and video.

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
