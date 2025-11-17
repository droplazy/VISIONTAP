#include "app_tiktok.h"
#include "machine_vision.h"
#include "opencv_utils.h"
#include <iostream>
#include <stdio.h>
#include "screen_tap.h"
APP_TIKTOK::APP_TIKTOK() : running(true) {
    // 构造函数
    turnon_application(APP_TIKTOK_ENUM);

}

APP_TIKTOK::~APP_TIKTOK() {
    // 确保析构时停止线程
    if (t.joinable()) {
        stop();
    }
}


bool APP_TIKTOK::checkAPKRunning(std::string apk_name)
{
    // 使用绝对路径调用 ps 命令，并且使用 -F 关闭正则表达式
    std::string command = "/bin/ps -ef | grep -v grep | grep -F \"" + apk_name + "\"\n";
  //  std::cout << "Command: " << command << std::endl;  // 打印调试信息，查看命令

    FILE* fp = popen(command.c_str(), "r");
    if (fp == nullptr) {
        perror("popen failed");
        return false; // 如果无法执行命令，返回 false
    }

    usleep(100000);  // 增加延迟时间

    char buffer[256];
    bool isRunning = false;

    // 读取命令输出，检查传入的 apk_name 是否在运行
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
      //  printf("buffer: %s\n", buffer);  // 打印调试信息，查看输出内容
        // 检查输出的行是否包含 apk_name
        if (strstr(buffer, apk_name.c_str()) != nullptr) {
            isRunning = true; // APK 正在运行
            break; // 找到该进程后可以退出循环
        }
    }

    fclose(fp);
    return isRunning; // 返回是否找到对应的进程
}

void APP_TIKTOK::start() {
    // 启动线程
    t = std::thread(&APP_TIKTOK::run, this);  // 创建线程并绑定到成员函数
}

void APP_TIKTOK::stop() {
    // 停止线程
    running = false;
    if (t.joinable()) {
        t.join();  // 等待线程结束
    }
}
void APP_TIKTOK::ContentExtraction()
{
    running = checkAPKRunning(APK_TIKTOK_NAME);

    snap_screen();
    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat templateImage = cv::imread("/data/machine_vision/apppic/living.png"); // 读取模板图像
    double score;
    ad_point match = FindPicTarget(targetImage, templateImage, score);

    if( score > 0.8)
    {
        contentType =LIVE_STREAMING;
    }
    else
    {
        contentType =SHORT_VIDEO;
    }
    if (contentType == LIVE_STREAMING) {
       // std::cout << "当前内容: 直播" << std::endl;
    } else if (contentType == SHORT_VIDEO) {
       // std::cout << "当前内容: 短视频" << std::endl;
    }

    // 打印进程状态
    if (running) {
      //  std::cout << "进程: 正常运行" << std::endl;
    } else {
        std::cout << "进程: 停止" << std::endl;
    }
}

void APP_TIKTOK::SearchPersonZone()
{
    INPUT_TYPEINGTEXT("神田川");

}

int APP_TIKTOK::SendComment(string comments)
{
    // if(contentType != SHORT_VIDEO)
    // {
    //     cout << "error : not short video connot comment !\n";
    //     return -1;
    // }

    if(CopyTextFormSys("text:杭州今天下小雨azxcxz!@##!@112233") <0 )
    {
        cout << "error : 无法复制文本!\n";
        return -1;
    }

    turnon_application(APP_TIKTOK_ENUM);

    ad_point clickP = TIKTOK_OPT_COMMENTS;
    LONG_DELAY;

    INPUT_TAP(clickP);
    LONG_DELAY;

    //打开评论区
    int ret = FindTargetClick(TIKTOK_COMMENTS_CV, false);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_COMMENTS_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;
    //长点击准备复制

    ret = FindTargetClick(TIKTOK_COMMENTS_CV, true);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_COMMENTS_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;
    //点击粘贴
    ret = FindTargetClick(TEXTPASTE_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << TEXTPASTE_PATH << "   NOT FOUND !" << endl;
        return -1;
    }

    //点击粘贴
    ret = FindTargetClick(TIKTOK_PRESSSEND_CV, false);
        if(ret < 0)
    {
        cout << "warning :" << TIKTOK_PRESSSEND_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    cout << "process has done\n";
    return 0;
}
void APP_TIKTOK::run() {
    // 线程执行的内容
    std::cout << std::boolalpha;
    while (1) {
       // std::cout << "RUNNING" << std::endl;  // 每秒打印一次 RUNNING
        ContentExtraction();
        this_thread::sleep_for(chrono::seconds(1));  // 等待一秒
        SendComment("WO SHIZHANG JIAHAO ");

    }
}
