#include "machine_vision.h"
#include "screen_tap.h"


void test_function()
{
    // 读取图片
    cv::Mat img = cv::imread("test.jpg");

    // 检查图片是否读取成功
    if (img.empty()) {
        printf("Could not open or find the image!\n");
        return ;
    }

    // 转换为灰度图
    cv::Mat gray_img;
    cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
	system("input touchscreen tap  200 200 \r\n");
    // 应用 box filter
    cv::Mat out;
    cv::boxFilter(gray_img, out, -1, cv::Size(5, 5));

    // 保存输出图片
    cv::imwrite("result.jpg", out);

    printf("Mission completed!\r\n");
}