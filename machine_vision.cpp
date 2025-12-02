#include "machine_vision.h"
#include "screen_tap.h"
#include <iostream>

#include <filesystem>
#include <chrono>
#include <thread>
#include <string>

#if 0
#include <jni.h>
// 用来获取 Java 环境的引用
JNIEnv* env;  // 这是从 JNI 中传递的环境对象
jobject context;  // Android 的 Context 对象

// JNI 方法：复制文本到剪贴板
void copyToClipboard(const std::string& text) {
    cout << "debugdebug debug \n" << endl;
    jclass cls = env->FindClass("com/example/yourapp/ClipboardHelper");
    jmethodID mid = env->GetStaticMethodID(cls, "copyToClipboard", "(Landroid/content/Context;Ljava/lang/String;)V");

    jstring jText = env->NewStringUTF(text.c_str());
    env->CallStaticVoidMethod(cls, mid, context, jText);
    env->DeleteLocalRef(jText);
}

// JNI 方法：从剪贴板获取文本
std::string getFromClipboard() {
    jclass cls = env->FindClass("com/example/yourapp/ClipboardHelper");
    jmethodID mid = env->GetStaticMethodID(cls, "getFromClipboard", "(Landroid/content/Context;)Ljava/lang/String;");
    jstring result = (jstring)env->CallStaticObjectMethod(cls, mid, context);

    const char* nativeString = env->GetStringUTFChars(result, 0);
    std::string str(nativeString);
    env->ReleaseStringUTFChars(result, nativeString);
    return str;
}
#endif
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
    /* SHORT_DELAY;
    OPEN_FILE_MANAGER() ;
    SHORT_DELAY;
*/
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
          //  printf("文件存在!\n");
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

ad_point FindTargetForDelay(string targetPng,double &score,int clycles)
{
    for (int var = 0; var < clycles; ++var)
    {
        ad_point match = FindTargetReturnPoint(targetPng);
        usleep(200*1000);
        if(match.x ==-1 || match.y ==-1)
        {
            //cout << "...\n" << endl;
        }
        else
        {
            // cout << "目标已找到 得分: " <<score << "\n" << endl;
            // cout << "坐标"<<match.x << ", " <<match.y<<" ...\n" <<endl;

            return match;
        }
    }
    cout << "警告:长时间未找到亩目标 已经退出...\n" << endl;

    return {-1,-1};
}


ad_point FindTargetReturnPoint(string targetPng)
{
    snap_screen();//重新生成一张背景图片
    checkFileExistsWithTimeout("/data/machine_vision/background.png",3);
    //LONG_DELAY;
    ad_point match = {-1,-1};


    // cout << "entrance3!!!!!!\n";
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
        return  {-1,-1};
    }



    double score;
    for (int i = 0; i < 1; ++i) {
        // 找到目标图片
        match = FindPicTarget(targetImage, templateImage, score);
        cout << "score :" <<score<< "wait times : " << i+1 <<"\n";

        // 如果匹配的分数小于0.8，返回-1
        if (score < 0.8 ) {
#if 1
            // 在目标图像中用红色框标记匹配区域
            cv::Rect matchRect(match.x, match.y, targetImage.cols, targetImage.rows);
            cv::Mat debugImage = templateImage.clone();  // 创建目标图像的副本

            // 用红色框住匹配区域
            cv::rectangle(debugImage, matchRect, cv::Scalar(0, 255, 0), 2); // 红色框

            // 计算矩形中心点
            cv::Point center(matchRect.x + matchRect.width / 2, matchRect.y + matchRect.height / 2);

            // 在矩形中心画一个红色点
            cv::circle(debugImage, center, 5, cv::Scalar(0, 0, 255), -1); // 红色点

            // 保存调试图像到本地
            cv::imwrite("debug_image_with_red_box.jpg", debugImage);
#endif

            return {-1,-1};
        }
        else
        {
            break;
        }
        // sleep(1);//
    }

    // 在目标图像中用红色框标记匹配区域
    cv::Rect matchRect(match.x, match.y, targetImage.cols, targetImage.rows);
#if 0
    cv::Mat debugImage = templateImage.clone();  // 创建目标图像的副本

    // 用红色框住匹配区域
    cv::rectangle(debugImage, matchRect, cv::Scalar(0, 255, 0), 2); // 红色框

    // 计算矩形中心点
    cv::Point center(matchRect.x + matchRect.width / 2, matchRect.y + matchRect.height / 2);

    // 在矩形中心画一个红色点
    cv::circle(debugImage, center, 5, cv::Scalar(0, 0, 255), -1); // 红色点

    // 保存调试图像到本地
    cv::imwrite("debug_image_with_red_box.jpg", debugImage);
#endif
    match ={matchRect.x + matchRect.width / 2, matchRect.y + matchRect.height / 2};

    return match;

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
    ad_point clickP ;
    ad_point sendMsg  ;
    ad_point match;
    double score;
    int var=0;

    match = FindTargetForDelay(FILESYSTEM_PATH,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "无法打开文件系统...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "打开文件系统成功.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        INPUT_TAP(match);
    }

    match = FindTargetForDelay(PIPETXT_PATH,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "无法打开管道文件系统...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "打开管道文件成功.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        INPUT_TAP(match);
    }//TEXTURE_CLICK ALLSELECT_PATH

    match = FindTargetForDelay(PIPETXT_LOCATE_PATH,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "无法定位管道文件系统...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "定位成功.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        match.x -= 100;
        match.y += 30;
        clickP = match;
        INPUT_TAP_DELAY(clickP,750);
    }//TEXTURE_CLICK

    match = FindTargetForDelay(ALLSELECT_PATH,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "无法复制...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "复制文本成功.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        INPUT_TAP(match);
    }
    INPUT_TAP_DELAY(clickP,750);

    match = FindTargetForDelay(TEXTCOPY_PATH,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "无法复制...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "复制文本成功.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        INPUT_TAP(match);
    }

    INPUT_BACK();

    match = FindTargetForDelay(PIPETXT_PATH,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "无法找到管道文件...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "找到管道文件准备删除.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        INPUT_TAP_DELAY(match,750);
    }

    match = FindTargetForDelay(DELFILE_PATH,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "无法找到删除按钮...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "找到管删除按钮.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        INPUT_TAP(match);
    }
    match = FindTargetForDelay(DELFILECERTAIN_PATH,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "无法找到删除确认按钮...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "删除完成.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        INPUT_TAP(match);
    }
    return 0;
}
