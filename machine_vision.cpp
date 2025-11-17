#include "machine_vision.h"
#include "screen_tap.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
// 打开指定的应用
void turnon_application(AppType apptype)
{
    switch (apptype)
    {
    case APP_TIKTOK_ENUM:
        std::cout << "DEBUG: Turning on TikTok..." << std::endl;
        TURN_ON_TIKTOK();
        break;

    case APP_WECHAT_ENUM:
        std::cout << "DEBUG: Turning on WeChat..." << std::endl;
        TURN_ON_WECHAT();
        break;

    case APP_REDBOOK_ENUM:
        std::cout << "DEBUG: Turning on RedBook..." << std::endl;
        TURN_ON_REDBOOK();
        break;

    case APP_MEITUAN_ENUM:
        std::cout << "DEBUG: Turning on Meituan..." << std::endl;
        TURN_ON_MEITUAN();
        break;

    case APP_DAZHONGDIANPING_ENUM:
        std::cout << "DEBUG: Turning on Da Zhong Dian Ping..." << std::endl;
        TURN_ON_DAZHONGDIANPING();
        break;

    case APP_BAIDU_ENUM:
        std::cout << "DEBUG: Turning on Baidu..." << std::endl;
        TURN_ON_BAIDU();
        break;

    case APP_ALIPAY_ENUM:
        std::cout << "DEBUG: Turning on Alipay..." << std::endl;
        TURN_ON_ALIPAY();
        break;

    case APP_QQ_ENUM:
        std::cout << "DEBUG: Turning on QQ..." << std::endl;
        TURN_ON_QQ();
        break;

    case APP_KUAISHOU_ENUM:
        std::cout << "DEBUG: Turning on Kuaishou..." << std::endl;
        TURN_ON_KUAISHOU();
        break;

    case APP_BILIBILI_ENUM:
        std::cout << "DEBUG: Turning on Bilibili..." << std::endl;
        TURN_ON_BILIBILI();
        break;

    default:
        std::cout << "DEBUG: Unknown application" << std::endl;
        break;
    }
}



// 启动 TikTok
void TURN_ON_TIKTOK(void)
{
    // TODO: 填充启动 TikTok 的逻辑
    std::cout << "Launching TikTok..." << std::endl;
    INPUT_HOME();
    SHORT_DELAY;
    OPEN_FILE_MANAGER() ;
    SHORT_DELAY;

    snap_screen();//重新生成一张背景图片
    FindTargetClick(TIKTOK_PATH,false);
}

// 启动 WeChat
void TURN_ON_WECHAT(void)
{
    // TODO: 填充启动 WeChat 的逻辑
    std::cout << "Launching WeChat..." << std::endl;
}

// 启动 RedBook
void TURN_ON_REDBOOK(void)
{
    // TODO: 填充启动 RedBook 的逻辑
    std::cout << "Launching RedBook..." << std::endl;
}

// 启动 Meituan
void TURN_ON_MEITUAN(void)
{
    // TODO: 填充启动 Meituan 的逻辑
    std::cout << "Launching Meituan..." << std::endl;
}

// 启动 大众点评
void TURN_ON_DAZHONGDIANPING(void)
{
    // TODO: 填充启动 大众点评 的逻辑
    std::cout << "Launching Da Zhong Dian Ping..." << std::endl;
}

// 启动 Baidu
void TURN_ON_BAIDU(void)
{
    // TODO: 填充启动 Baidu 的逻辑
    std::cout << "Launching Baidu..." << std::endl;
}

// 启动 Alipay
void TURN_ON_ALIPAY(void)
{
    // TODO: 填充启动 Alipay 的逻辑
    std::cout << "Launching Alipay..." << std::endl;
}

// 启动 QQ
void TURN_ON_QQ(void)
{
    // TODO: 填充启动 QQ 的逻辑
    std::cout << "Launching QQ..." << std::endl;
}

// 启动 Kuaishou
void TURN_ON_KUAISHOU(void)
{
    // TODO: 填充启动 Kuaishou 的逻辑
    std::cout << "Launching Kuaishou..." << std::endl;
}

// 启动 Bilibili
void TURN_ON_BILIBILI(void)
{
    // TODO: 填充启动 Bilibili 的逻辑
    std::cout << "Launching Bilibili..." << std::endl;
}

void INPUT_TYPEINGTEXT(string text)
{
    system("/storage/emulated/0/Download/PIPEFILE.txt");

    ofstream outFile("/storage/emulated/0/Download/PIPEFILE.txt", std::ios::app);

    // 检查文件是否成功打开
    if (outFile.is_open()) {
        // 格式化写入
        outFile <<text << std::endl;// "Text: " <<
        outFile.close(); // 关闭文件
    } else {
        std::cerr << "无法打开文件进行写入" << std::endl;
    }
}
#include <stdio.h>
#include <unistd.h>  // 用于 access 函数
#include <time.h>    // 用于时间处理

int checkFileExistsWithTimeout(const char* filePath, int timeoutSeconds) {
    time_t startTime, currentTime;

    // 获取当前时间
    time(&startTime);

    while (1) {
        // 检查文件是否存在
        if (access(filePath, F_OK) == 0) {
            printf("文件存在!\n");
            return 1; // 文件存在
        }

        // 获取当前时间
        time(&currentTime);

        // 检查是否超时
        if (difftime(currentTime, startTime) >= timeoutSeconds) {
            printf("超时!\n");
            return 0; // 超时
        }

        // 等待100毫秒再继续检查
        usleep(100000);  // 100毫秒
    }
}


int FindTargetClick(string targetPng,bool clickdelay)
{
    snap_screen();//重新生成一张背景图片
    checkFileExistsWithTimeout("/data/machine_vision/background.png",3);
    LONG_DELAY;
    cv::Mat targetImage;
    cv::Mat templateImage;
    int i;
    for ( i = 0; i < 5; ++i) {
        // 读取目标图像
        targetImage = cv::imread(targetPng);
        if (targetImage.empty()) {
            std::cerr << "无法读取目标图像: " << i<<targetPng << std::endl;
            continue;
        }

        // 读取模板图像
        templateImage = cv::imread("/data/machine_vision/background.png");
        if (templateImage.empty()) {
            std::cerr << "无法读取模板图像" << i << std::endl;
            continue; // 如果读取模板图像失败，返回-1
        }
        break;
    }
    if(i >4)
    {
        cerr << " 无法生成cv ：mat";
        return  -1;
    }

    double score;
    ad_point match ;
    for (int i = 0; i < 3; ++i) {
        // 找到目标图片
        match = FindPicTarget(targetImage, templateImage, score);

        // 如果匹配的分数小于0.8，返回-1
        if (score < 0.8 ) {
            if(i >= 2)
            {
            cout << "score :" <<score<< "\n bad score"<<"\n" << "wait times : " << i+1 <<"\n";
            return -1;
            }
        }
        else
        {
            break;
        }
        sleep(1);//
    }


    // 如果分数大于等于0.8，计算点击位置并执行点击
    match.x += APPUI_XY / 2;
    match.y += APPUI_XY / 2;
    if(clickdelay)
    {
        INPUT_TAP_DELAY(match,1500);
         sleep(2);
    }
    else
    {
            INPUT_TAP(match);
    }
    cout << "click point :" <<match.x <<","<< match.y  \
         << "score :" <<score << "\n" \
         << "targetPic :" <<targetPng << "\n";
    return 0; // 匹配成功，返回0
}

int CopyTextFormSys(string texture)
{
    INPUT_TYPEINGTEXT(texture);
    INPUT_HOME();
    SHORT_DELAY;
    OPEN_FILE_MANAGER();
    SHORT_DELAY;
    ScreenTapDownToUp();
    SHORT_DELAY;
    //打开文件
    int ret = FindTargetClick(FILESYSTEM_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << FILESYSTEM_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;
    //打开pipe文档
    ret = FindTargetClick(PIPETXT_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << PIPETXT_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;
#if 0
    //点击文本内容
    ret = FindTargetClick(TEXTCONTENT_PATH, true);
    if(ret < 0)
    {
        cout << "warning :" << TEXTCONTENT_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
#else
    LONG_DELAY;

    ad_point clickP = TEXTURE_CLICK;
    INPUT_TAP_DELAY(clickP,1000);
    LONG_DELAY;
#endif




    // LONG_DELAY;
    //点击全选
    ret = FindTargetClick(ALLSELECT_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << ALLSELECT_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;

    //点击复制
    ret = FindTargetClick(TEXTCOPY_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << TEXTCOPY_PATH << "   NOT FOUND !" << endl;
        return -1;
    }

    INPUT_BACK();
    LONG_DELAY;//长延时等剪切板提示消失
    //长点击pipe文档
    ret = FindTargetClick(PIPETXT_PATH, true);
    if(ret < 0)
    {
        cout << "warning :" << PIPETXT_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
    SHORT_DELAY;
    //删除文本文件
    ret = FindTargetClick(DELFILE_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << DELFILE_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
    SHORT_DELAY;
    //删除确定
    ret = FindTargetClick(DELFILECERTAIN_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << DELFILECERTAIN_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
    return 0;
}
