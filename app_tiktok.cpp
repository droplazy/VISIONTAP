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
    std::cout << std::boolalpha;

    //    running = checkAPKRunning(APK_TIKTOK_NAME);

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
        std::cout << "当前内容: 直播     "<<std::endl;
    } else if (contentType == SHORT_VIDEO) {
        std::cout << "当前内容: 短视频   " << std::endl;
    }

}

void APP_TIKTOK::CheckUpgrade()
{
    snap_screen();

    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat templateImage = cv::imread(TIKTOK_LATER_CV); // 读取模板图像
    double score;
    ad_point match = FindPicTarget(targetImage, templateImage, score);
    if(score >0.8)
    {
        cout << "检测到升级提示\n"<< endl;
        INPUT_TAP(match);
    }

    return ;
}

bool APP_TIKTOK::ShowMyHomepage()
{
    ad_point clickp= TIKTOK_OPT_I;
    INPUT_TAP(clickp);
    LONG_DELAY;



    snap_screen();

    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat templateImage = cv::imread(TIKTOK_HOMEPAGE_CV); // 读取模板图像
    double score;
    ad_point match = FindPicTarget(targetImage, templateImage, score);
    if(score >0.8)
    {
        cout << "进入个人主页成功\n"<< endl;
        return true;

    }
    cout << "进入个人主页失败\n"<< endl;

    return false;
}

bool APP_TIKTOK::CheckLaunching()
{
    snap_screen();

    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat templateImage = cv::imread(TIKTOK_LAUNCHING_CV); // 读取模板图像
    double score;
    ad_point match = FindPicTarget(targetImage, templateImage, score);
    if(score >0.8)
    {
        cout << "应用启动中....\n"<< endl;
        return false;

    }

    cout << "应用启动成功....\n"<< endl;

    return true;
}


void APP_TIKTOK::beatBack(int cnt)
{
    for (int var = 0; var < cnt; ++var) {
        INPUT_BACK();
        SHORT_DELAY;
    }

}

void APP_TIKTOK::FollowMode(string FollowText,int circleTimes)
{


    // if(CopyTextFormSys(FollowText) <0 )
    // {
    //     cout << "错误无法复制文本!\n";
    //     return -1;
    // }
    //  turnon_application(APP_TIKTOK_ENUM);
    bool isEnter= false;
    int ret ;
    for (int sec = 0; sec < circleTimes; ++sec)
    {
        if(!isEnter)
        {
            ret = EntranceLivingRoom("互涨1000粉");//进入指定直播间
            if(ret == -1)
            {
                cout << "无法进入直播间\n"<< endl;
                beatBack(6);
                continue;
            }
            else
            {
                cout << "进入直播间成功\n"<< endl;
                SHORT_DELAY;
                if(CopyTextFormSys(FollowText) <0 )
                {
                    cout << "error : 无法复制文本!\n";
                    return ;
                }
                turnon_application(APP_TIKTOK_ENUM);

                isEnter =true;
                // exit(0);
            }
        }




        int i=0;
        for (i = 0; i < 5; ++i)
        {
            cout << "检查直播间三要素 >>>......\n" << endl;

            if( isLivingRoom())
            {
                cout << "确认完毕 >>>......\n" << endl;
                break;
            }
        }
        if(i >=4)
        {
            isEnter =false;
            continue;
        }


        for (int var = 0; var < 1; ++var) {
            RandomFollowUser();
        }
        SHORT_DELAY;
        ret = SendBraggerForLivingRoom(FollowText,true);
        if ( ret== -2)
        {
            isEnter =false;
        }
        SHORT_DELAY;
        //   randomCickScreen();
    }


    return;

}

bool APP_TIKTOK::SearchShortVelement(ad_point &like,ad_point &comment ,ad_point &farvour,ad_point &forward,double &finalscore)
{
    snap_screen();
    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat simple =  cropImage(targetImage,973,188,41,250);//190
    double score;
    cv::Mat templateImage = cv::imread("/data/machine_vision/apppic/shortvideo_ele.png");
    ad_point match = FindPicTargetWithMask(simple, templateImage,templateImage, score);
    if(score< 0.8 )
    {
        cout << "score:" << score << " bad score,try another template\n" <<endl;

        templateImage = cv::imread("/data/machine_vision/apppic/shortvideo_ele_2.png");
        match = FindPicTargetWithMask(simple, templateImage,templateImage, score);
        if(score< 0.8 )
        {
            finalscore =score;
            return false;
        }
    }
    cout << "score:" << score << "match: "<< match.x << "," <<match.y<<"\n" <<endl;
    like.x =  match.x+13+973;
    comment.x =  match.x+13+973;
    farvour.x =  match.x+13+973;
    forward.x =  match.x+13+973;
    like.y =  match.y+8+188;
    comment.y =  match.y+61+188;
    farvour.y =  match.y+116+188;
    forward.y =  match.y+168+188;
    finalscore =score;
    return true;
}

void APP_TIKTOK::ScrollingShortVideos(int clycles)
{
    // ContentExtraction();
    ad_point like ={0,0};
    ad_point comment ={0,0};
    ad_point farvour ={0,0};
    ad_point forward ={0,0};
    double score =0.0f;
    int lowScoreCnt=0;
    auto start = std::chrono::high_resolution_clock::now();
    for(int i =0;i<clycles*999999;i++)
    {
        // 获取当前时间点
        auto end = std::chrono::high_resolution_clock::now();

        // 计算时间差，单位为毫秒
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
     //   std::cout << "五秒钟后上滑视频" << duration.count() << " milliseconds" << std::endl;
        if(duration.count() >6000)
        {
            // 输出时间差
            std::cout << "6秒钟后上滑视频" << duration.count() << " milliseconds" << std::endl;

            auto start = std::chrono::high_resolution_clock::now();
            scrollingUP();
        }
        if(SearchShortVelement(like,comment,farvour,forward,score))
        {
            cout <<"该内容可以被点赞   给予点赞\n" << endl;
            INPUT_TAP(like);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        /*    scrollingUP();
            auto start = std::chrono::high_resolution_clock::now();*/
        }
        else if( score < 0.3)
        {
            snap_screen();
            cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
            cv::Mat templateImage = cv::imread("/data/machine_vision/apppic/living.png"); // 读取模板图像
            double score;
            ad_point match = FindPicTarget(targetImage, templateImage, score);

            if( score > 0.8)
            {
                cout <<"刷到了直播内容\n" << endl;
                auto start = std::chrono::high_resolution_clock::now();

                scrollingUP();
            }
            else
            {
                templateImage = cv::imread(TIKTOK_LIVING_UI_2_CV); // 读取模板图像
                ad_point match = FindPicTarget(targetImage, templateImage, score);
                if( score > 0.8)
                {
                    cout <<"刷到了直播内容\n" << endl;
                    auto start = std::chrono::high_resolution_clock::now();

                    scrollingUP();
                }else
                {
                cout <<"未知内容...\n" << endl;
                lowScoreCnt++;
                }
            }
        }

    }

    //cv::imwrite("simple.jpg", simple);

    // cv::Mat maskPic = cv::imread("/data/machine_vision/apppic/shortvideo_ele.png", cv::IMREAD_GRAYSCALE);  // 读取目标图像
    // cv::Mat masked = Maskpicture(simple,maskPic);

    // cv::imwrite("masked.jpg", masked);

    cout << "endend \n" <<endl;

    /*cv::Mat templateImage = cv::imread("/data/machine_vision/apppic/shortvideo_ele.png"); // 读取模板图像
    cv::Mat Maskmage = cv::imread("/data/machine_vision/apppic/shortvideo_ele_grey.png"); // 读取模板图像

    double score;
    ad_point match = FindPicTargetWithMask(targetImage, templateImage,templateImage, score);
    cout << "score:" << score << "match : " << match.x << " ," <<match.y <<"  \n" <<endl;*/



    /*
    if(contentType == LIVE_STREAMING)
    {
        scrollingUP();
    }
    else
    {
        sleep(2);
        VideoContentOPT(GIVELIKE_OPT);
        sleep(2);
        VideoContentOPT(FAVOURITE_OPT);
        sleep(2);
    }*/
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

void APP_TIKTOK::VideoContentOPT(CONTENT_OPT operat)
{
    ad_point clickP ={0,0};


    switch (operat) {
    case GIVELIKE_OPT:
    {clickP=TIKTOK_OPT_LIKES;INPUT_TAP(clickP);}
    break;
    case COMMENT_OPT:

        break;
    case FAVOURITE_OPT:
    {clickP=TIKTOK_OPT_FARVOUR;INPUT_TAP(clickP);}
    break;
    case FORWARD_OPT:

        break;
    default:
        break;
    }
}

bool APP_TIKTOK::VideoContentForward()
{
    return false;
}

void APP_TIKTOK::randomCickScreen()
{
    ad_point clickP={512,300};
    INPUT_TAP(clickP);
    usleep(100*1000);
    clickP={500,310};
    INPUT_TAP(clickP);
    usleep(100*1000);
    clickP={520,250};
    INPUT_TAP(clickP);
    usleep(100*1000);
    clickP={525,322};
    INPUT_TAP(clickP);
    usleep(100*1000);
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
    ad_point match;
    ad_point clickP;
    for (int var = 0; var < 3; ++var) {
        match = FindTargetReturnPoint(TIKTOK_LIVING_UI_CV);//todo
        if(match.x== -1 || match.y== -1)
        {

            match = FindTargetReturnPoint(TIKTOK_LIVING_UI_2_CV);//todo
            if(match.x== -1 || match.y== -1)
            {
                cout << "找不到直播间入口" <<endl;

                if(var>=2)
                    return -1;
                else
                {
                    cout << "继续搜索" <<endl;

                    clickP = TIKTOK_OPT_CONTINUE_SEARCH;
                    INPUT_TAP_DELAY(clickP,1000);
                    continue;
                }
            }

        }
    }
    LONG_DELAY;
    clickP = match;
    INPUT_TAP(clickP);
    SHORT_DELAY;

    // ad_point match = FindTargetReturnPoint(TIKTOK_LIVING_ROOM_CV);//todo
    // cout << "Is living room ： " <<isLivingRoom() << "\n " << endl;

    return 0;
}
void APP_TIKTOK::RandomFollowUser()
{
    for (int i = 0; i < 5; ++i) {
        cout << "检查直播间三要素 >>>......\n" << endl;

        if( isLivingRoom())
        {
            cout << "确认完毕 >>>......\n" << endl;
            break;
        }
        else
        {
            cout << "未能确认 返回......\n" << endl;
            return ;
        }
        //   LONG_DELAY;
    }


    ad_point clickP = {56,464};
    INPUT_TAP(clickP);
    LONG_DELAY;
    for (int var = 0; var < 3; ++var) {

        cout << "检查关注界面n" <<endl;
        ad_point match = FindTargetReturnPoint(TIKTOK_LIVING_FOLLOW_UI_CV);//todo
        if(match.x== -1 || match.y== -1)
        {
            cout << "未发现关注按钮\n" <<endl;
            match = FindTargetReturnPoint(TIKTOK_LIVING_FOLLOWED_UI_CV);//todo
            if(match.x>0&& match.y>0)
            {
                cout << "该用户已经关注\n"<<endl;
                ad_point clickP = {56,464};
                INPUT_TAP(clickP);
                SHORT_DELAY;

            }
            else if(var >=2)
            {
                cout << "一直未发现关注按钮\n" <<endl;
                return;
            }
            else
            {
                // cout << "继续点击一次\n" <<endl;
                // ad_point clickP = {56,464};
                // INPUT_TAP(clickP);
                // LONG_DELAY;
            }
            continue;
        }
        else
        {
            clickP = match;
            INPUT_TAP(clickP);
            cout << "已经随机一名用户\n" << endl;
            LONG_DELAY;
            ad_point clickP = {56,464};
            INPUT_TAP(clickP);
            SHORT_DELAY;
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
#if 0
    for (int i = 0; i < 5; ++i) {
        cout << "检查直播间三要素 >>>......\n" << endl;

        if( isLivingRoom())
        {
            cout << "确认完毕 >>>......\n" << endl;
            break;
        }
        else
        {
          return -2;
        }
     //   LONG_DELAY;
    }
#endif
    ad_point clickP ;
    ad_point match;
    for(int var = 0; var < 5; ++var)
    {
        clickP = TIKTOK_OPT_BARRAGE_BUTTON;
        INPUT_TAP(clickP);
        usleep(1200*1000);

        match = FindTargetReturnPoint(TIKTOK_LIVING_KEYBOARD_UI_CV);//todo
        if(match.x== -1 || match.y== -1)
        {
            cout << "等待键盘弹出" <<endl;

            continue;
        }
        else
        {
            cout << "键盘已经弹出" <<endl;
            break;
        }
    }


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

    for (int var = 0; var < 5; ++var)
    {


        SHORT_DELAY;
        match.x -= 250;
        clickP = match;
        INPUT_TAP(clickP);//点击编辑栏目
        SHORT_DELAY;
        INPUT_TAP_DELAY(clickP,500);//长按编辑栏目
        SHORT_DELAY;
        match = FindTargetReturnPoint(TIKTOK_LIVINGROOM_BRAGGER_PASTE_CV);//todo
        if(match.x== -1 || match.y== -1)
        {
            cout << "warining : cannot paste at living room " <<endl;
            // cout << "warining : cannot paste at living room " <<endl;
            // cout << "warining : cannot paste at living room " <<endl;//TODO
            // cout << "warining : cannot paste at living room " <<endl;
            // cout << "warining : cannot paste at living room " <<endl;
            // sleep(3);
            match = FindTargetReturnPoint(TIKTOK_PRESSSEND_CV);//todo
            continue ;
        }
        else
        {
            cout << "准备点击粘贴按钮\n"<<endl;
            break;
        }
    }

    clickP = match;
    INPUT_TAP(clickP);

    match = FindTargetReturnPoint(TIKTOK_PRESSSEND_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "warining : cannot locate editor" <<endl;
        return -1;
    }
    // LONG_DELAY;
    clickP = match;
    INPUT_TAP(clickP);
    //LONG_DELAY;

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
        cout << "未发现小心心按钮\n" <<endl;
    }
    else
        eleGet ++;

    match = FindTargetReturnPoint(TIKTOK_LIVING_ELE_2_UI_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "未发现礼物按钮\n" <<endl;
    }
    else
        eleGet ++;

    match = FindTargetReturnPoint(TIKTOK_LIVING_ELE_3_UI_CV);//todo
    if(match.x== -1 || match.y== -1)
    {
        cout << "未发现直播广场按钮\n" <<endl;
    }
    else
        eleGet ++;

    match = FindTargetReturnPoint(TIKTOK_LIVING_TERMINATE_UI_CV);//todo
    if(match.x>0&& match.y>0)
    {
        eleGet =0;
        cout << "直播已经终止\n"<<endl;
    }
    else
        eleGet ++;


    if(eleGet >=2)
        ret=true;
    else
        ret = false;

    //  LONG_DELAY;

    return ret;
}
bool APP_TIKTOK::LaunchToHomepage()
{
    if(!CheckLaunching())
    {
        return false;
    }
    else
    {
        CheckUpgrade();
        bool isEnterHomePage =false;
        int var=0;
        for ( var = 0; var < 10; ++var)
        {
            isEnterHomePage =ShowMyHomepage();
            if(isEnterHomePage)
            {
                break;
            }
            usleep(500*1000);
        }
        if(var >=10)
        {
            cout << "未能进入主页.....\n"<< endl;
            return false;
        }
        else
        {
            cout << "成功进入主页.....\n"<< endl;
            return true;
        }
    }
}

void APP_TIKTOK::run()
{
    // 线程执行的内容
    while (1)
    {
        /*   if(!running)
        {
            if(LaunchToHomepage())
            {
                INPUT_BACK();
                running =true;
            }
            else
            {
                continue;
            }
        }*/

        ScrollingShortVideos(5000);

    }

}
int getRandomInRange(int min, int max) {
    return rand() % (max - min + 1) + min;
}
void APP_TIKTOK::scrollingUP()
{
    // 给start和end坐标添加随机抖动
    ad_point start = {512 + getRandomInRange(-12, 12), 300 + getRandomInRange(-10, 10)};
    ad_point end = {512 + getRandomInRange(-12, 12), 50 + getRandomInRange(-10, 10)};

    // 给duration添加随机抖动
    int duration = getRandomInRange(450, 650);

    // 执行滑动操作
    INPUT_SWIPE(start, end, duration);
}

void APP_TIKTOK::scrollingDown()
{
    // 给start和end坐标添加随机抖动
    ad_point start = {512 + getRandomInRange(-12, 12), 50 + getRandomInRange(-10, 10)};
    ad_point end = {512 + getRandomInRange(-12, 12), 300 + getRandomInRange(-10, 10)};

    // 给duration添加随机抖动
    int duration = getRandomInRange(450, 650);

    // 执行滑动操作
    INPUT_SWIPE(start, end, duration);
}
//FollowMode("关必回🪅🪅🪅🪅🪅关必回🪅🪅🪅🪅🪅",100);
// RandomFollowUser();
// SendBraggerForLivingRoom("关必回🪅🪅🪅🪅🪅关必回🪅🪅🪅🪅🪅",true);

// for (int var = 0; var < 1; ++var) {
//     RandomFollowUser();
// }
// RandomFollowUser();
// if(SendBraggerForLivingRoom("关必回🪅🪅🪅🪅🪅关必回🪅🪅🪅🪅🪅",true))
// {
//      beatBack(6);
//     LONG_DELAY;
//      EntranceLivingRoom();
// }
