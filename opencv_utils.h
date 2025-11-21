#ifndef OPENCV_UTILS_H
#define OPENCV_UTILS_H

#include "pulic_def.h"
#include "opencv2/opencv.hpp"  // 引入 OpenCV 库

void snap_screen();
ad_point FindPicTarget(cv::Mat targetImage, cv::Mat templateImage, double &Score);
ad_point FindPicTargetWithMask(cv::Mat targetImage, cv::Mat templateImage,cv::Mat MaskImage, double &Score);
cv::Mat cropImage(const cv::Mat& src, int x, int y, int width, int height);//裁减一张图片
cv::Mat Maskpicture(cv::Mat Source,cv::Mat maskB);

void test_opencv();

#endif // OPENCV_UTILS_H
