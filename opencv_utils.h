#ifndef OPENCV_UTILS_H
#define OPENCV_UTILS_H

#include "pulic_def.h"
#include "opencv2/opencv.hpp"  // 引入 OpenCV 库

void snap_screen();
ad_point FindPicTarget(cv::Mat targetImage, cv::Mat templateImage, double &Score);
void test_opencv();

#endif // OPENCV_UTILS_H
