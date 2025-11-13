#include "opencv_utils.h"

void test_opencv()
{
    snap_screen();//重新生成一张背景图片

    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat templateImage = cv::imread("/data/machine_vision/tiktok.png"); // 读取模板图像
    double score;
    ad_point match = FindPicTarget(targetImage, templateImage, score);

    if (match.x != -1 && match.y != -1) {
        std::cout << "模板匹配成功，左上角坐标: (" << match.x << ", " << match.y << "), 得分: " << score << std::endl;
    } else {
        std::cout << "未找到有效匹配。" << std::endl;
    }

}


void snap_screen()//重新生成背景图片
{
    system("rm /data/machine_vision/background.png");
    usleep(10*1000);
    system("screencap -p /data/machine_vision/background.png");
}

ad_point FindPicTarget(cv::Mat targetImage, cv::Mat templateImage, double &Score)
{
    // 检查图像是否为空
    if (targetImage.empty() || templateImage.empty()) {
        std::cerr << "无法加载图片" << std::endl;
        return ad_point{-1, -1};  // 返回一个无效的坐标
    }

    // 转换为灰度图
    cv::Mat targetGray, templateGray;
    cv::cvtColor(targetImage, targetGray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(templateImage, templateGray, cv::COLOR_BGR2GRAY);

    // 执行模板匹配
    cv::Mat result;
    cv::matchTemplate(targetGray, templateGray, result, cv::TM_CCOEFF_NORMED);

    // 找到最匹配的位置
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // 返回匹配得分
    Score = maxVal;

    // 如果得分太低，返回一个无效的坐标
    if (Score < 0.8) {
        return ad_point{-1, -1};
    }

    // 返回匹配位置的左上角坐标
    ad_point matchPoint = {maxLoc.x, maxLoc.y};

// 调试部分：标记匹配区域并保存图像
#if 1
    // 在目标图像中用红色框标记匹配区域
    cv::Rect matchRect(maxLoc.x, maxLoc.y, templateImage.cols, templateImage.rows);
    cv::Mat debugImage = targetImage.clone();  // 创建目标图像的副本

    // 用红色框住匹配区域
    cv::rectangle(debugImage, matchRect, cv::Scalar(0, 0, 255), 2); // 红色框

    // 保存调试图像到本地
    cv::imwrite("debug_image_with_red_box.jpg", debugImage);
#endif

    return matchPoint;
}



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
