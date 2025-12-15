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
    //usleep(1500*1000);

}
// std::string getTimestamp() {
//     std::time_t now = std::time(nullptr);
//     std::tm* tm = std::localtime(&now);
//     char buffer[20];
//     std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", tm);
//     return std::string(buffer);
// }

ad_point FindPicTargetWithMask(cv::Mat targetImage, cv::Mat templateImage, cv::Mat MaskImage, double &Score)
{
    // 检查图像是否为空
    if (targetImage.empty() || templateImage.empty() || MaskImage.empty()) {
        std::cerr << "无法加载图片或掩模图" << std::endl;
        return ad_point{-1, -1};  // 返回一个无效的坐标
    }



    // 转换为灰度图
    cv::Mat targetGray, templateGray,MaskGray;
    cv::cvtColor(targetImage, targetGray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(templateImage, templateGray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(MaskImage, MaskGray, cv::COLOR_BGR2GRAY);

    // // 检查掩模图像大小与目标图像是否一致
    // if (MaskImage.size() != targetImage.size()) {
    //     std::cerr << "掩模图像大小与目标图像不一致！"<<  << std::endl;
    //     return ad_point{-1, -1};
    // }
    // 执行模板匹配，使用掩模图像
    cv::Mat result;
    cv::matchTemplate(targetGray, templateGray, result, cv::TM_CCOEFF_NORMED, MaskGray);

    // 找到最匹配的位置
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // 返回匹配得分
    Score = maxVal;

    // 调试部分：标记匹配区域并保存图像 13 8 61 116 168
#if 1
    // 在目标图像中用红色框标记匹配区域
    cv::Rect matchRect(maxLoc.x, maxLoc.y, templateImage.cols, templateImage.rows);
    cv::Mat debugImage = targetImage.clone();  // 创建目标图像的副本

    // 用红色框住匹配区域
    cv::rectangle(debugImage, matchRect, cv::Scalar(0, 0, 255), 2); // 红色框
/*
    // 以红框的左上角为原点，画四个绿色点
    std::vector<cv::Point> points = {
        cv::Point(matchRect.x + 13, matchRect.y + 8),   // 点1
        cv::Point(matchRect.x + 13, matchRect.y + 61),  // 点2
        cv::Point(matchRect.x + 13, matchRect.y + 116), // 点3
        cv::Point(matchRect.x + 13, matchRect.y + 168)  // 点4
    };

    // 在指定的点上画上绿色的圆点
    for (const auto& point : points) {
        cv::circle(debugImage, point, 5, cv::Scalar(0, 255, 0), -1);  // 绿色点，半径为5，-1表示填充
    }
    */
    // 保存调试图像到本地
    static int sequenceNumber = 0; // 静态变量用于跟踪序号
    sequenceNumber++; // 每次调用接口时+1

    std::string timestamp = getTimestamp();

    std::string debugImageFilename = "debug_image_with_red_box_" + timestamp + "_" + std::to_string(sequenceNumber) + ".jpg";
    std::string targetImageFilename = "cropImage_" + timestamp + "_" + std::to_string(sequenceNumber) + ".jpg";

    std::cout << "ele_score: " << Score << " X: " << maxLoc.x << " Y: " << maxLoc.y << " " << timestamp << " Sequence: " << sequenceNumber << std::endl;

    cv::imwrite(debugImageFilename, debugImage);
    cv::imwrite(targetImageFilename, targetImage);

#endif

    // 如果得分太低，返回一个无效的坐标
    if (Score < 0.8) {
        return ad_point{-1, -1};
    }

    // 返回匹配位置的左上角坐标
    ad_point matchPoint = {maxLoc.x, maxLoc.y};

    return matchPoint;
}

// // 1. 读取图像A和图像B
// cv::Mat A = cv::imread("imageA.jpg");  // 彩色图像
// cv::Mat B = cv::imread("imageB.jpg", cv::IMREAD_GRAYSCALE);  // 黑白图像
cv::Mat Maskpicture(cv::Mat Source,cv::Mat maskB)
{
    // 确保源图像和遮罩图像大小一致
    if (Source.size() != maskB.size()) {
        std::cerr << "Error: Source and mask must have the same size!" << std::endl;
        return cv::Mat();
    }

    // 确保 maskB 是单通道图像
    if (maskB.channels() != 1) {
        std::cerr << "Error: maskB should be a single-channel image!" << std::endl;
        return cv::Mat();
    }

    // 创建一个与 Source 相同大小的空白图像
    cv::Mat result = cv::Mat::zeros(Source.size(), Source.type());

    // 对于 maskB 中白色区域（255）保留 Source 图像的内容，黑色区域（0）置为 0
    Source.copyTo(result, maskB);  // 只复制 maskB 为 255 的部分

    return result;
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
    // 如果得分太低，返回一个无效的坐标
    if (Score < 0.8) {
        return ad_point{-1, -1};
    }

    // 返回匹配位置的左上角坐标
    ad_point matchPoint = {maxLoc.x, maxLoc.y};



    return matchPoint;
}

// 简单的图像裁剪函数
cv::Mat cropImage(const cv::Mat& src, int x, int y, int width, int height) {
    if (src.empty()) {
        std::cerr << "Error: Source image is empty!" << std::endl;
        return cv::Mat();
    }

    // 越界检查
    if (x < 0 || y < 0 || x + width > src.cols || y + height > src.rows) {
        std::cerr << "Error: Crop region out of bounds!" << std::endl;
        return cv::Mat();
    }

    cv::Rect roi(x, y, width, height);
    return src(roi).clone(); // clone 防止引用原图
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
