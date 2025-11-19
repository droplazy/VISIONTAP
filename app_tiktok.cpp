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

void APP_TIKTOK::beatBack(int cnt)
{
    for (int var = 0; var < cnt; ++var) {
        INPUT_BACK();
        SHORT_DELAY;
    }

}

int APP_TIKTOK::FollowMode(string FollowText,int circleTimes)
{
    EntranceLivingRoom("交朋友直播");//进入指定直播间
    if(CopyTextFormSys(FollowText) <0 )
    {
        cout << "error : 无法复制文本!\n";
        return -1;
    }

    for (int sec = 0; sec < circleTimes; ++sec) {
        RandomFollowUser();
        SendBraggerForLivingRoom(FollowText,true);
    }


    return 0;

}

int APP_TIKTOK::SearchPersonZone(string Name)
{
    if(CopyTextFormSys(Name) <0 )
    {
        cout << "error : 无法复制文本!\n";
        return -1;
    }

    turnon_application(APP_TIKTOK_ENUM);
    LONG_DELAY;
    LONG_DELAY;
    ad_point clickP = TIKTOK_OPT_SEARCH;
    INPUT_TAP(clickP);
    LONG_DELAY;


    clickP = TIKTOK_OPT_TEXT_INPUT;
    INPUT_TAP_DELAY(clickP,1000);
    LONG_DELAY;

    //粘贴
    int ret = FindTargetClick(TEXTPASTE_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << TEXTPASTE_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;

    clickP = TIKTOK_OPT_CONTINUE_SEARCH;
    INPUT_TAP_DELAY(clickP,1000);

    LONG_DELAY;



   /* clickP = TIKTOK_OPT_FIRST_ELEMENT;
    INPUT_TAP_DELAY(clickP,1000);

    LONG_DELAY;*/
    return 0;
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

    //点击发送
    ret = FindTargetClick(TIKTOK_PRESSSEND_CV, false);
        if(ret < 0)
    {
        cout << "warning :" << TIKTOK_PRESSSEND_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    cout << "process has done\n";
    return 0;
}
/*  clickStart = {512,500};
    clickEnd = {512,200};

    INPUT_SWIPE(clickStart,clickEnd,500);  //上划
    SHORT_DELAY;*/
int APP_TIKTOK::VideoContentLike(string name,string message)
{
    SearchPersonZone(name);

    //点击用户按钮
    int ret = FindTargetClick(TIKTOK_SEARCH_USER_CV, false);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_SEARCH_USER_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;

    TAP_SWIPE_DOWN() ;
    SHORT_DELAY;
     TAP_SWIPE_UP() ;
    SHORT_DELAY;

    ad_point match = FindTargetReturnPoint(TIKTOK_MAKEING_CV);//todo
    //TODO 找到作品的按钮
    return 0;

}

int APP_TIKTOK::EntranceLivingRoom(string name)
{
    SearchPersonZone(name);
#if 1
    //点击直播按钮
    int ret = FindTargetClick(TIKTOK_SEARCH_LIVING_CV, false);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_SEARCH_LIVING_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;
#else
    //点击用户按钮
    int ret = FindTargetClick(TIKTOK_SEARCH_USER_CV, false);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_SEARCH_USER_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;
#endif
    ad_point match = FindTargetReturnPoint(TIKTOK_LIVING_UI_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "warining : "<<TIKTOK_LIVING_UI_CV<<"not found" <<endl;
        return -1;
    }
    LONG_DELAY;
    ad_point clickP = match;
    INPUT_TAP(clickP);
    SHORT_DELAY;

   // ad_point match = FindTargetReturnPoint(TIKTOK_LIVING_ROOM_CV);//todo
    cout << "Is living room ： " <<isLivingRoom() << "\n " << endl;

    return 0;
}
void APP_TIKTOK::RandomFollowUser()
{
    ad_point clickP = {56,464};
    INPUT_TAP(clickP);

    for (int var = 0; var < 3; ++var) {
        ad_point match = FindTargetReturnPoint(TIKTOK_LIVING_FOLLOW_UI_CV);//todo
        if(match.x== -1 || match.y== -1)
        {
            cout << "warining : "<<TIKTOK_LIVING_FOLLOW_UI_CV<<"not found"<<"cnt:"<<var<<"\n" <<endl;
        }
        else
        {
            clickP = match;
            INPUT_TAP(clickP);
            cout << "FOLLOW SOME ONE success!\n" << endl;

            ad_point clickP = {56,464};
            INPUT_TAP(clickP);
            break;
        }
    }
}

int APP_TIKTOK::SendBraggerForLivingRoom(string message,bool noEdit)
{
    if(!noEdit)
    {
        if(CopyTextFormSys(message) <0 )
        {
            cout << "error : 无法复制文本!\n";
            return -1;
        }
        turnon_application(APP_TIKTOK_ENUM);
    }

    for (int i = 0; i < 5; ++i) {
        cout << "waiiting app activing >>>......\n" << endl;

        if( isLivingRoom())
        {
            break;
        }
        LONG_DELAY;
    }

    ad_point clickP = TIKTOK_OPT_BARRAGE_BUTTON;
    INPUT_TAP(clickP);
    LONG_DELAY;
    LONG_DELAY;

    ad_point match;
    for (int var = 0; var < 5; ++var) {
        match = FindTargetReturnPoint(TIKTOK_PRESSSEND_CV);//todo
        if(match.x== -1 || match.y== -1)
        {
            cout << "warining : cannot locate editor" <<endl;
            sleep(1);

            continue;
        }
        else
            break;

    }

    LONG_DELAY;
    match.x -= 250;
    clickP = match;
    INPUT_TAP(clickP);
    SHORT_DELAY;

    INPUT_TAP_DELAY(clickP,500);
    LONG_DELAY;
    match = FindTargetReturnPoint(TIKTOK_LIVINGROOM_BRAGGER_PASTE_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "warining : cannot paste at living room " <<endl;
        return -1;
    }
    LONG_DELAY;
    //点击粘贴
    clickP = match;
    INPUT_TAP(clickP);
    LONG_DELAY;

    match = FindTargetReturnPoint(TIKTOK_PRESSSEND_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "warining : cannot locate editor" <<endl;
        return -1;
    }
    LONG_DELAY;
    clickP = match;
    INPUT_TAP(clickP);
    LONG_DELAY;

    //cout << "waitting send commder ....\n" <<endl;

    return 0;
}

int APP_TIKTOK::FollowSpecifiedUser(string name)
{
    SearchPersonZone(name);


    //点击用户按钮
    int ret = FindTargetClick(TIKTOK_SEARCH_USER_CV, false);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_SEARCH_USER_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;

    ad_point clickP = TIKTOK_OPT_FOLLOW_BUTTON;
    INPUT_TAP(clickP);
    LONG_DELAY;

    return 0;
}



int APP_TIKTOK::SendMessageToPerson(string name,string message)
{
    int ret = SearchPersonZone(name);
    if(ret < 0)
    {
        cout << "warning :" <<"search failed\n" << endl;
        return -1;
    }
        LONG_DELAY;
    //点击用户按钮
    ret = FindTargetClick(TIKTOK_SEARCH_USER_CV, false);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_SEARCH_USER_CV << "   NOT FOUND !" << endl;
        return -1;
    }

    LONG_DELAY;
    ad_point clickP = TIKTOK_OPT_ZONE_BUTTON;
    INPUT_TAP(clickP);
    LONG_DELAY;

    if(CopyTextFormSys(message) <0 )
    {
        cout << "error : 无法复制文本!\n";
        return -1;
    }

    turnon_application(APP_TIKTOK_ENUM);
    LONG_DELAY;
    //
     ret = FindTargetClick(TIKTOK_SEND_MESSAGE_CV, false);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_SEND_MESSAGE_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    LONG_DELAY;
    clickP = TIKTOK_OPT_CLICK_DIALOG_1;
    INPUT_TAP(clickP);
    LONG_DELAY;
    clickP = TIKTOK_OPT_CLICK_DIALOG_2;
    INPUT_TAP_DELAY(clickP,500);
    SHORT_DELAY;
    //点击粘贴ints
    ret = FindTargetClick(TEXTPASTE_PATH, false);
    if(ret < 0)
    {
        cout << "warning :" << TEXTPASTE_PATH << "   NOT FOUND !" << endl;
        return -1;
    }
    SHORT_DELAY;

    ret = FindTargetClick(TIKTOK_SEND_MESSAGE_UI_CV, false);
    if(ret < 0)
    {
        cout << "warning :" << TIKTOK_SEND_MESSAGE_UI_CV << "   NOT FOUND !" << endl;
        return -1;
    }
    SHORT_DELAY;
    return 0;
}

bool APP_TIKTOK::isLivingRoom()
{
    bool ret = false;
    int eleGet=0;
    ad_point match = FindTargetReturnPoint(TIKTOK_LIVING_ELE_1_UI_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "warining : "<<TIKTOK_LIVING_ELE_1_UI_CV<<"not found" <<endl;
    }
    else
        eleGet ++;

    match = FindTargetReturnPoint(TIKTOK_LIVING_ELE_2_UI_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "warining : "<<TIKTOK_LIVING_ELE_2_UI_CV<<"not found" <<endl;
    }
    else
        eleGet ++;

    match = FindTargetReturnPoint(TIKTOK_LIVING_ELE_3_UI_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "warining : "<<TIKTOK_LIVING_ELE_3_UI_CV<<"not found" <<endl;
    }
    else
        eleGet ++;


    if(eleGet >=2)
        ret=true;
    else
        ret = false;

    LONG_DELAY;

    return ret;
}
void APP_TIKTOK::run() {
    // 线程执行的内容
    std::cout << std::boolalpha;
    while (1) {
        this_thread::sleep_for(chrono::seconds(1));  // 等待一秒
        //FollowMode("🈶关必回🈶关必回🈶关必回🈶关必回🈶关必回😍😍😍😍😍",60);
        SendMessageToPerson("神田川","我还活我还活着我还活着🙍🙍😍😍😍😍😍🦸🏇");
        beatBack(6);
    }
}
