#include "Thread_Tikok.h"
#include "machine_vision.h"
#include "opencv_utils.h"
#include <iostream>
#include <stdio.h>
#include "screen_tap.h"


#if 0

Thread_Tikok::Thread_Tikok()
    : ThreadBase("Thread_Tikok", Dev_Action())  // 调用父类的两个参数构造函数
{
    std::cout << "Thread_Tikok 默认构造函数" << std::endl;
}
#endif

// 带参数的构造函数
Thread_Tikok::Thread_Tikok(const std::string& name, const Dev_Action& action)
    : ThreadBase(name, action)  // 将参数传递给父类
{
    std::cout << "Thread_Tikok 构造函数: " << name << std::endl;
    parseText(action.remark);
}


Thread_Tikok::~Thread_Tikok() {
    // 确保析构时停止线程
    if (t.joinable()) {
        stop();
    }
}

void Thread_Tikok::ContentExtraction()
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
        std::cout << "当前内容: 直播     "<<std::endl;
    }
    else
    {
        std::cout << "当前内容: 短视频   " << std::endl;
    }

}

void Thread_Tikok::CheckFirstLaunch()
{
    snap_screen();

    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat templateImage = cv::imread(TIKTOK_CONTENT_FIRSTLAUNCH_CV); // 读取模板图像
    double score;
    ad_point match = FindPicTarget(targetImage, templateImage, score);
    if(score >0.8)
    {
        cout << "第一次启动该应用  或者应用已经省级\n"<< endl;
        match.x +=145;
        match.y +=280;

        INPUT_TAP(match);
    }

    return ;
}
void Thread_Tikok::CheckUpgrade()
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
bool Thread_Tikok::checklogin()
{
    snap_screen();

    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat templateImage = cv::imread("/data/machine_vision/apppic/tiktoklogin.png"); // 读取模板图像
    double score;
    ad_point match = FindPicTarget(targetImage, templateImage, score);
    if(score >0.8)
    {
        cout << "账号需要登陆\n"<< endl;
        //  INPUT_TAP(match);
        return true ;
    }
    else
    {
        cv::Mat templateImage = cv::imread("/data/machine_vision/apppic/login2.png"); // 读取模板图像
        double score;
        ad_point match = FindPicTarget(targetImage, templateImage, score);
        if(score >0.8)
        {
            cout << "账号需要登陆\n"<< endl;
            //  INPUT_TAP(match);
            return true ;
        }
    }
    return false;
}

bool Thread_Tikok::ShowMyHomepage()
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

bool Thread_Tikok::CheckLaunching()
{
    snap_screen();

    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat templateImage = cv::imread(TIKTOK_LAUNCHING_CV); // 读取模板图像
    double score;
    ad_point match = FindPicTarget(targetImage, templateImage, score);
    if(score >0.8)
    {
        cout << "应用启动中...."<<score << endl;
        return false;

    }

    cout << "应用启动成功....\n"<< endl;

    return true;
}


void Thread_Tikok::beatBack(int cnt)
{
    for (int var = 0; var < cnt; ++var) {
        INPUT_BACK();
        SHORT_DELAY;
    }

}

void Thread_Tikok::FollowMode(string FollowText,string roomname,int circleTimes)
{


    // if(CopyTextFormSys(FollowText) <0 )
    // {
    //     cout << "错误无法复制文本!\n";
    //     return -1;
    // }
    //  turnon_application(APP_TIKTOK_ENUM);
    bool isEnter= false;
    bool ProhibitFollow= false;

    int ret ;
    for (int sec = 0; sec < circleTimes; ++sec)
    {
        if(!isEnter)
        {
            ret = EntranceLivingRoom(roomname);//进入指定直播间
            if(ret == -1)
            {
                cout << "无法进入直播间\n"<< endl;
                beatBack(6);
                CheckUpgrade();
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

                int i=0;
                for (i = 0; i < 15; ++i)
                {
                    cout << "检查直播间三要素 >>>......\n" << endl;
                    int ret = isLivingRoom() ;
                    if( ret>=3)
                    {
                        cout << "确认完毕 >>>......\n" << endl;
                        break;
                    }
                    else if(ret <0)
                    {
                        isEnter =false;
                        beatBack(5);
                        continue;
                    }
                }
                if(i >=4)
                {
                    isEnter =false;
                    beatBack(5);
                    continue;
                }
                isEnter =true;
                // exit(0);
            }
        }


        if(sec %10 == 0)
        {
#if 1
            for (int i = 0; i < 5; ++i) {
                cout << "检查直播间三要素 >>>......\n" << endl;

                if( isLivingRoom() >=3)
                {
                    cout << "确认完毕 >>>......\n" << endl;
                    break;
                }
                else
                {
                    cout << "未能确认 返回......\n" << endl;
                    return ;
                }
            }
#endif
        }


        if(!ProhibitFollow)
        {
            if(RandomFollowUser() == -2)
            {
                isEnter =false;
            }
            else if(RandomFollowUser() == -3)
            {
                ProhibitFollow =true;
                beatBack(2);
            }
        }
        SHORT_DELAY;
        ret = SendBraggerForLivingRoom(FollowText,true);
        if ( ret== -2)
        {
            isEnter =false;
            beatBack(5);
        }
        SHORT_DELAY;
        //   randomCickScreen();
    }


    return;

}

bool Thread_Tikok::SearchShortVelement(ad_point &like,ad_point &comment ,ad_point &farvour,ad_point &forward,double &finalscore)
{
    snap_screen();
    cv::Mat targetImage = cv::imread("/data/machine_vision/background.png");  // 读取目标图像
    cv::Mat simple =  cropImage(targetImage,973,130,41,300);//190
    double score;
    cv::Mat templateImage = cv::imread("/data/machine_vision/apppic/shortvideo_ele.png");
    ad_point match = FindPicTargetWithMask(simple, templateImage,templateImage, score);
    if(score< 0.8 )
    {
        cout << "ele 1 _ score:" << score << " bad score,try another template\n" <<endl;

        templateImage = cv::imread("/data/machine_vision/apppic/shortvideo_ele_2.png");
        match = FindPicTargetWithMask(simple, templateImage,templateImage, score);
        if(score< 0.8 )
        {

            cout << "ele 2 _ score:" << score << " bad score,try another template\n" <<endl;
            match = FindPicTargetWithMask(simple, templateImage,templateImage, score);
            if(score< 0.8 )
            {

                cout << "ele 3 _ score:" << score << " bad score,try another template\n" <<endl;


                finalscore =score;
                return false;
            }
            else
            {
                cout << "score:" << score << "match: "<< match.x << "," <<match.y<<"\n" <<endl;
                like.x =  match.x+15+973;
                comment.x =  match.x+15+973;
                farvour.x =  match.x+15+973;
                forward.x =  match.x+15+973;
                like.y    =  match.y+14+130;
                comment.y =  match.y+73+130;
                farvour.y =  match.y+135+130;
                forward.y =  match.y+189+130;
                finalscore =score;
            }

        }
        else
        {
            cout << "score:" << score << "match: "<< match.x << "," <<match.y<<"\n" <<endl;
            like.x =  match.x+15+973;
            comment.x =  match.x+15+973;
            farvour.x =  match.x+15+973;
            forward.x =  match.x+15+973;
            like.y    =  match.y+11+130;
            comment.y =  match.y+69+130;
            farvour.y =  match.y+131+130;
            forward.y =  match.y+168+130;
            finalscore =score;

        }
    }
    else
    {
        cout << "score:" << score << "match: "<< match.x << "," <<match.y<<"\n" <<endl;
        like.x =  match.x+13+973; //973 和188 为小范围模板图片在大图中的位置
        comment.x =  match.x+13+973;
        farvour.x =  match.x+13+973;
        forward.x =  match.x+13+973;
        like.y =  match.y+8+188;
        comment.y =  match.y+61+188;
        farvour.y =  match.y+116+188;
        forward.y =  match.y+168+188;
        finalscore =score;
    }
    if(isnan(score))
    {
        return false;
    }

    return true;
}

void Thread_Tikok::ScrollingShortVideos(int clycles)
{
    // ContentExtraction();
    ad_point like ={0,0};
    ad_point comment ={0,0};
    ad_point farvour ={0,0};
    ad_point forward ={0,0};
    double score =0.0f;
    int lowScoreCnt=0;
    auto start = std::chrono::high_resolution_clock::now();
    for(int i =0;i<clycles;i++)
    {



        // 获取当前时间点
        auto end = std::chrono::high_resolution_clock::now();

        // 计算时间差，单位为毫秒
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        //   std::cout << "五秒钟后上滑视频" << duration.count() << " milliseconds" << std::endl;
        if(duration.count() >6000 && 0)
        {
            // 输出时间差
            std::cout << "6秒钟后上滑视频" << duration.count() << " milliseconds" << std::endl;

            auto start = std::chrono::high_resolution_clock::now();
            scrollingUP();
        }
        if(SearchShortVelement(like,comment,farvour,forward,score))
        {
            cout <<"图像对比 通过\n" << endl;
            // 生成1到16之间的随机数
            srand(time(0));
            int randomNum = rand() % 16 + 1;
            cout <<"操作种子:"<< randomNum <<"\n" << endl;

            if(randomNum & 0x01)
            {
                cout <<"给予点赞\n" << endl;
                RandomShortVideoOperation(like,GIVELIKE_OPT,"");
            }
            usleep(500*1000);
            if(randomNum>>1 & 0x01)
            {
                cout <<"给予评论\n" << endl;

                RandomShortVideoOperation(comment,COMMENT_OPT,"🌹🌹🌹");
                beatBack(5);
            }
            usleep(500*1000);

            if(randomNum>>2 & 0x01)
            {
                cout <<"给予收藏\n" << endl;

                RandomShortVideoOperation(farvour,FAVOURITE_OPT,"");
            }
            usleep(500*1000);

            if(randomNum>>3 & 0x01)
            {
                cout <<"给予转发\n" << endl;

                RandomShortVideoOperation(forward,FORWARD_OPT,"");
                //   beatBack(5);
            }
            usleep(500*1000);
            scrollingUP();

        }
        else
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
                    cout <<"未知内容..点击1下back.\n" << endl;
                    //lowScoreCnt++;
                    INPUT_BACK();
                }
            }
        }

    }
    cout << "endend \n" <<endl;

}

int Thread_Tikok::SpecifyContentOperation(string link,CONTENT_OPT opt,string comment)
{
    ad_operations click_p={0};

    int ret = enterSpecifyContent(link,click_p);
    SHORT_DELAY;
    if(ret != 0)
    {
        return ret ;
    }
    if(!ret && (opt&GIVELIKE_OPT))
    {
        RandomShortVideoOperation(click_p.like,GIVELIKE_OPT,"");
        SHORT_DELAY;
    }
    if(!ret && (opt&FAVOURITE_OPT))
    {
        RandomShortVideoOperation(click_p.favour,FAVOURITE_OPT,"");
    }
    if(!ret && (opt&FORWARD_OPT))
    {
        RandomShortVideoOperation(click_p.forward,FORWARD_OPT,"");
    }
    if(!ret && (opt&COMMENT_OPT))
    {
        RandomShortVideoOperation(click_p.comment,COMMENT_OPT,comment);
    }
    beatBack(5);
    return 0;
}

int Thread_Tikok::SpecifyLivingRoomOnSite(string link)
{
    if(enterSpecifyLivingrom(link) <0)
    {
        cout << "进入直播间失败 ...." <<endl;
        return -1;
    }
    return 0;
}

void Thread_Tikok::TaskUpdate(Dev_Action task)
{
    parseText(task.remark);
    selectTaskPreExec();
    action = task;

    cout << "活动已经更新   --> " <<  action.sub_action<< endl;
}

void Thread_Tikok::parseText(const string &text)
{
    ContentExtractor extractor;
    auto [id, link, msg,mark] = extractor.extractContent(action.remark);

    this->remark_link = link;
    this->remark_id = id;
    this->remark_mark = mark;
    this->remark_msg = msg;
#if 0
    std::cout << "remark1: " << this->remark << std::endl;
    std::cout << "id: " << id << std::endl;
    std::cout << "link: " << link << std::endl;
    std::cout << "msg: " << msg << std::endl;
    std::cout << "mark: " << mark << std::endl;
#endif

}

bool Thread_Tikok::onAppStart()
{
    applacationstate = AppState::STARTING;
    turnon_application(APP_TIKTOK_ENUM);
    while(1)
    {
        if(!isLogin)
        {
            cout << "账号需要登陆 ...." << endl;
            return  false;
        }

        int var =0;
        for (var = 0; var < 5; ++var)
        {
            if(LaunchToHomepage())
            {
                INPUT_BACK();
                applacationstate = AppState::IDLE;
                selectTaskPreExec();

                return true;
            }

            CheckUpgrade();
            CheckFirstLaunch();
            checklogin();
        }
        if(var >=4)
        {
            cout << "应用启动异常" <<endl;
            applacationstate = AppState::ERROR;
            return false;
        }
    }

}

void Thread_Tikok::onAppExit()
{


    INPUT_TASKAPP();
    double score;
    ad_point match = FindTargetForDelay(TIKTOK_APPATTASK_CV,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "没有找到图标...\n" <<endl;
        INPUT_HOME();
        return;
    }
    ad_point start = match;
    start.y+=400;
    INPUT_SWIPE(start,match,220);
    cout << "APP已经完全退出" <<endl;
    INPUT_HOME();

}

void Thread_Tikok::onStateChanged(ThreadState newState)
{

}

bool Thread_Tikok::hasTask() //这是轮询标志
{

    if(TASK_EXEC != TASK_UNKNOW && TASK_EXEC != TASK_NONE&& TASK_EXEC != TASK_COMPELTED)
        return true;
    else if(TASK_EXEC==TASK_COMPELTED)
    {
        applacationstate = AppState::EXITING;
    }

    return false;
}



void Thread_Tikok::executeTask()
{
    applacationstate = AppState::BUSY;

    if(TASK_EXEC == TASK_SEND_MESSAGE)
    {
        SendMessageToPerson(remark_id,remark_msg);
        // beatBack(10);
        // INPUT_HOME();

        TASK_EXEC = TASK_COMPELTED;
    }
    else if(TASK_EXEC == TASK_QUIT)
    {
        // beatBack(10);
        // INPUT_HOME();
     //   applacationstate = AppState::EXITING;
        TASK_EXEC = TASK_COMPELTED;
    }
    else if(TASK_EXEC == TASK_FOLLOW_MODE)
    {
        for (int i = 0; i < 3; ++i)
        {
            cout << "检查直播间三要素 >>>......\n" << endl;

            if( isLivingRoom() >=3)
            {
                cout << "确认完毕 >>>......\n" << endl;
                break;
            }
            else if (i>=2)
            {
                SpecifyLivingRoomOnSite(remark_link);
                break;
            }
        }

        SendBraggerForLivingRoom(remark_msg,false);
        ad_point tap_cli ={612,20};//空点击
        INPUT_TAP(tap_cli);
        TASK_EXEC =TASK_FOLLOW_MODE_RUNNING;
    }
    else if(TASK_EXEC == TASK_FOLLOW_MODE_RUNNING)
    {
        int i =0;
        for (i = 0; i < 3; ++i)
        {
            cout << "检查直播间三要素 >>>......\n" << endl;

            if( isLivingRoom()>=3)
            {
                cout << "确认完毕 >>>......\n" << endl;
                break;
            }
            else if(isLivingRoom() <0)
            {
                TASK_EXEC = TASK_COMPELTED;
            }
        }

        if (i>=2)
        {
            TASK_EXEC =TASK_FOLLOW_MODE;
            return ;
        }


        if(!ProhibitFollow_b)
        {
            if(RandomFollowUser() == -2)
            {
                TASK_EXEC =TASK_FOLLOW_MODE;
            }
            else if(RandomFollowUser() == -3)
            {
                ProhibitFollow_b =true;
                // beatBack(10);
                // INPUT_HOME();
                TASK_EXEC = TASK_COMPELTED;
            }
        }
        SendBraggerForLivingRoom(remark_msg,true);
    }
    else if(TASK_EXEC == TASK_SCROLLING_MODE)
    {
        ScrollingShortVideos(1);
    }
    else if(TASK_EXEC == TASK_LVIVINGROOM_ONSITE)
    {
        int ret = isLivingRoom();
            if( ret>=3)
            {
             //   cout << "正在直播间："<<remark_link << endl;
            }
            else if(ret <0)
            {
                TASK_EXEC =TASK_COMPELTED;
            }
            else
            {
                if(SpecifyLivingRoomOnSite(remark_link) ==-1)
                {
                    TASK_EXEC =TASK_COMPELTED;
                }
            }
    }
    else if(TASK_EXEC == TASK_LVIVINGROOM_BULLET)
    {
            cout << "检查直播间三要素 >>>......\n" << endl;
        int ret = isLivingRoom();
            if( ret >=3)
            {
                cout << "确认完毕 >>>......\n" << endl;
                SendBraggerForLivingRoom(remark_msg,false);
                TASK_EXEC = TASK_LVIVINGROOM_ONSITE;
            }
            else if(ret <0)
            {
                TASK_EXEC =TASK_COMPELTED;
            }
            else
            {
                SpecifyLivingRoomOnSite(remark_link);
            }


    }
    else if(TASK_EXEC == TASK_CONTENT_OPTRATION)
    {
        CONTENT_OPT opt= 0;
        if (remark_mark.find("点赞") != std::string::npos) {
            std::cout << "需要点赞 .. " << std::endl;
            opt |=GIVELIKE_OPT;
        }
        if (remark_mark.find("评论") != std::string::npos) {
            std::cout << "需要评论 .. " << std::endl;
            opt |=COMMENT_OPT;
        }
        if (remark_mark.find("收藏") != std::string::npos) {
            std::cout << "需要收藏 .. " << std::endl;
            opt |=FAVOURITE_OPT;
        }
        if (remark_mark.find("转发") != std::string::npos) {
            std::cout << "需要转发 .. " << std::endl;
            opt |=FORWARD_OPT;
        }
        if(SpecifyContentOperation(remark_link,opt,remark_msg) !=  0)
        {

            TASK_EXEC = TASK_COMPELTED;
        }
        // beatBack(10);
        // INPUT_HOME();
        TASK_EXEC = TASK_COMPELTED;
    }
    else
    {
        std::cout << "未知内容: " << action.remark << std::endl;
        applacationstate = AppState::IDLE;
    }
}
void Thread_Tikok::ContentForward()
{
    double score;
    ad_point match={0};
    match = FindTargetForDelay(TIKTOK_CONTENT_SHARE_CV,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "找不到分享界面...\n" <<endl;
        return ;
    }

    cout << "分享界面已经找到 准备寻找群聊...\n" <<endl;


    match = FindTargetForDelay(TIKTOK_CONTENT_SPEICYTALK_2_CV,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "找不到指定群聊...\n" <<endl;
    }
    else
    {
        cout << "点击程序员交流群\n"<< endl;
        INPUT_TAP(match);
        match = FindTargetForDelay(TIKTOK_CONTENT_SHARESEND_CV,score,15);
        if(match.x == -1 || match.y == -1)
        {
            cout << "找不到发送按钮...\n" <<endl;
        }
        INPUT_TAP(match);

        cout << "点击发送\n"<< endl;
        SHORT_DELAY;
        return;
    }

    match.y+= 100;

    ad_point swipe_end ={match.x-200,match.y};
    ad_point swipe_start =match;

    for (int var = 0; var < 10; ++var)
    {
        INPUT_SWIPE(swipe_start,swipe_end,750);
        match = FindTargetForDelay(TIKTOK_CONTENT_SHAREMORE_CV,score,1);
        if(match.x == -1 || match.y == -1)
        {
            cout << "继续寻找更多...\n" <<endl;
            cout << "滑动坐标"<<swipe_start.x << ", " <<swipe_start.y<<" ...\n" <<endl;
            cout << "滑动坐标"<<swipe_end.x << ", " <<swipe_end.y<<" ...\n" <<endl;

        }
        else
        {
            break;
        }
    }
    SHORT_DELAY;
    cout << "目标已找到:"<<match.x << ":"<< match.y<<"...\n" <<endl;
    INPUT_TAP(match);

    match = FindTargetForDelay(TIKTOK_CONTENT_MUILTYTALK_CV,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "找不到群聊界面...\n" <<endl;
        return;
    }
    cout << "点击群聊分享\n"<< endl;
    INPUT_TAP(match);

    match = FindTargetForDelay(TIKTOK_CONTENT_SPEICYTALK_CV,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "找不到指定群聊...\n" <<endl;
    }
    cout << "点击程序员交流群\n"<< endl;
    INPUT_TAP(match);

    match = FindTargetForDelay(TIKTOK_CONTENT_SHARESEND_CV,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "找不到发送按钮...\n" <<endl;
    }
    INPUT_TAP(match);

    cout << "点击发送\n"<< endl;
    SHORT_DELAY;
}
void Thread_Tikok::ContentComment(string content)
{

    CopyTextFormSys(content);
    turnon_application(APP_TIKTOK_ENUM);
    int var =0;
    for ( var = 0; var < 5*3; ++var)
    {
        ad_point match = FindTargetReturnPoint("/data/machine_vision/apppic/commentAear.png");
        usleep(200*1000);
        if(match.x ==-1 || match.y ==-1)
        {
            match = FindTargetReturnPoint("/data/machine_vision/apppic/commentAear_2.png");
            if(match.x ==-1 || match.y ==-1)
            {
                cout << "寻找编辑栏目\n" << endl;
            }
            else
            {
                cout << "点击编辑栏目\n" << endl;
                match.x-=300;
                INPUT_TAP(match);
                break;
            }
        }
        else
        {
            cout << "点击编辑栏目\n" << endl;
            match.x-=300;
            INPUT_TAP(match);
            break;
        }
    }
    if(var >14)
    {
        return ;
    }
    for ( var = 0; var < 5*3; ++var)
    {
        ad_point match = FindTargetReturnPoint(TIKTOK_LIVING_KEYBOARD_UI_CV);
        usleep(200*1000);
        if(match.x ==-1 || match.y ==-1)
        {
            cout << "寻找键盘\n" << endl;
        }
        else
        {
            cout << "键盘已经弹出\n" << endl;
            break;
        }
    }
    if(var >14)
    {
        return ;
    }
    for ( var = 0; var < 5*3; ++var)
    {
        ad_point match = FindTargetReturnPoint("/data/machine_vision/apppic/pressSend_2.png");
        usleep(200*1000);
        if(match.x ==-1 || match.y ==-1)
        {
            cout << "寻找输入区...1\n" << endl;
            ad_point match = FindTargetReturnPoint("/data/machine_vision/apppic/pressSend.png");
            usleep(200*1000);
            if(match.x ==-1 || match.y ==-1)
            {
                cout << "寻找输入区...2\n" << endl;
            }
            else
            {
                cout << "输入找到了\n" << endl;

                match.y -= 30;
                //   match.x -= 300;

                INPUT_TAP_DELAY(match,1000);

                break;
            }
        }
        else
        {

            match.y -= 40;
            //  match.x -= 300;
            cout << "输入找到了,调出粘贴按钮\n"  <<"x:"<< match.x << " y: " <<match.y<< endl;

            INPUT_TAP_DELAY(match,1000);

            break;
        }
    }
    if(var >14)
    {
        return ;
    }
    for ( var = 0; var < 5*3; ++var)
    {
        ad_point match = FindTargetReturnPoint(TEXTPASTE_PATH);
        usleep(200*1000);
        if(match.x ==-1 || match.y ==-1)
        {
            cout << "等待粘贴\n" << endl;
        }
        else
        {
            cout << "点击粘贴\n" << endl;
            INPUT_TAP(match);
            break;
        }
    }
    if(var >14)
    {
        return ;
    }
    for ( var = 0; var < 5*3; ++var)
    {
        ad_point match = FindTargetReturnPoint("/data/machine_vision/apppic/pressSend_2.png");
        usleep(200*1000);
        if(match.x ==-1 || match.y ==-1)
        {
            cout << "准备点击发送\n" << endl;
            ad_point match = FindTargetReturnPoint("/data/machine_vision/apppic/pressSend.png");
            usleep(200*1000);
            if(match.x !=-1 && match.y !=-1)
            {
                cout << "消息已经发送\n" << endl;
                INPUT_TAP(match);
                break;            }
        }
        else
        {
            cout << "消息已经发送\n" << endl;
            INPUT_TAP(match);
            break;
        }
    }
    if(var >14)
    {
        return ;
    }
}

void Thread_Tikok::RandomShortVideoOperation(ad_point click,CONTENT_OPT opt,string content)
{
    if(opt == GIVELIKE_OPT)
    {
        INPUT_TAP(click);
    }
    else if(opt == COMMENT_OPT)
    {
        INPUT_TAP(click);

        ContentComment(content);
        beatBack(1);
    }
    else if(opt == FAVOURITE_OPT)
    {
        INPUT_TAP(click);
    }
    else if(opt == FORWARD_OPT)
    {
        INPUT_TAP(click);
        ContentForward();

    }
}


int Thread_Tikok::SearchPersonZone(string Name)
{
#if 1
    if(CopyTextFormSys(Name) <0 )
    {
        cout << "error : 无法复制文本!\n";
        return -1;
    }

    turnon_application(APP_TIKTOK_ENUM);
#endif

    LONG_DELAY;
    double score;
    ad_point match={0};
    ad_point clickP = TIKTOK_OPT_SEARCH;
    ad_point searchClick = {0};
    int var =0;
    for ( var = 0; var < 5; ++var)
    {
        INPUT_TAP(clickP);
        match = FindTargetForDelay(TIKTOK_CONTENT_SEARCHPAGE_CV,score,3);
        if(match.x == -1 || match.y == -1)
        {
            cout << "没有打开搜索页面...\n" <<endl;
            match = FindTargetForDelay(TIKTOK_CONTENT_SEARCHPFILLED_CV,score,3);
            if(match.x == -1 || match.y == -1)
            {
                cout << "未知原因"<<var<<"\n" <<endl;

                continue;
            }
            else
            {
                cout << "搜索编辑栏有内容存在，点击X"<<var<<"\n" <<endl;
                match.x -= 25;
                INPUT_TAP(match);
                searchClick = match;
                searchClick.x +=50;
                usleep(100*1000);
                break;
            }


            continue;
        }
        else
        {
            cout << "搜索页面打开成功.."  <<match.x<<"," <<match.y<<".\n" <<endl;
            searchClick = match;
            searchClick.x +=20;
            break;
        }
    }
    if(var>=5)
    {
        cout << "搜索页面打开失败...\n" <<endl;
        return -1;
    }
    match.x -= 100;
    clickP= match;
    //    INPUT_TAP(match);
    cout << "点击编辑栏目准备粘贴...\n" <<endl;

    for ( var = 0; var < 5; ++var)
    {
        INPUT_TAP_DELAY(clickP,750);
        match = FindTargetForDelay(TEXTPASTE_PATH,score,5);
        if(match.x == -1 || match.y == -1)
        {
            cout << "没有找到粘贴页面...\n" <<endl;
            continue;
        }
        else
        {
            cout << "粘贴位置找到.." <<match.x<<"," <<match.y<<".\n" <<endl;
            INPUT_TAP(match);
            break;
        }
    }
    if(var>=5)
    {
        cout << "找不到粘贴的位置...\n" <<endl;
        return -1;
    }

    INPUT_TAP(searchClick);

    /*match = FindTargetForDelay(TIKTOK_CONTENT_SEARCHPAGE_CV,score,15);
    if(match.x == -1 || match.y == -1)
    {
        cout << "没有搜索按钮...\n" <<endl;
        return -1;
    }
    else
    {
        cout << "搜索按钮找到.." <<match.x<<"," <<match.y<<".\n" <<endl;
        match.x+=20;
        INPUT_TAP(match);
    }*/
    return 0;
}
int Thread_Tikok::enterSpecifyLivingrom(string content)
{
    if(CopyTextFormSys(content))
    {
        return -1;
    }

    turnon_application(APP_TIKTOK_ENUM);


    double score;
    ad_point match={0};
    // ad_point like ={0,0};
    // ad_point comment ={0,0};
    // ad_point farvour ={0,0};
    // ad_point forward ={0,0};
    int var =0;
    for ( var = 0; var < 3; ++var)
    {
        match = FindTargetForDelay(TIKTOK_CONTENT_SHAREDLINK_CV,score,5);
        if(match.x == -1 ||match.y == -1)
        {
            INPUT_HOME();
            SHORT_DELAY;
            turnon_application(APP_TIKTOK_ENUM);
            SHORT_DELAY;
            cout << "未能打开链接  重试.."<< endl;

        }
        else
        {
            match.y -= 10;
            INPUT_TAP(match);
            cout << "找到分享内容.."<< endl;

            break;
        }
    }
    if(var >2)
    {
        cout << "尝试手动打开内容.."<< endl;
        EntranceLivingRoom(content);
        // SearchPersonZone(content);
        // SHORT_DELAY;
    }


    int i=0;
    for (i = 0; i < 15; ++i)
    {
        cout << "检查直播间三要素 >>>......\n" << endl;

        if( isLivingRoom()>=3)
        {
            cout << "确认完毕 >>>......\n" << endl;
            return 0;
        }
    }
    if(i>14)
    {
        cout << "无法进入直播间 >>>......\n" << endl;

        return -1;
    }
    return 0;
}
int Thread_Tikok::enterSpecifyContent(string content ,ad_operations &opt_point)
{
    if(CopyTextFormSys(content))
    {
        return -1;
    }

    turnon_application(APP_TIKTOK_ENUM);


    double score;
    ad_point match={0};
    // ad_point like ={0,0};
    int var =0;
    for ( var = 0; var < 3; ++var)
    {
        match = FindTargetForDelay(TIKTOK_CONTENT_SHAREDLINK_CV,score,5);
        if(match.x == -1 ||match.y == -1)
        {
            INPUT_HOME();
            SHORT_DELAY;
            turnon_application(APP_TIKTOK_ENUM);
            SHORT_DELAY;
            cout << "未能打开链接  重试.."<< endl;
            CopyTextFormSys(content);
        }
        else
        {
            match.y -= 10;
            INPUT_TAP(match);
            cout << "找到分享内容.."<< endl;

            break;
        }
    }
    if(var >2)
    {
        cout << "尝试手动打开内容.."<< endl;

        SearchPersonZone(content);
        SHORT_DELAY;
    }
    for ( var = 0; var < 6; ++var) {
        if(SearchShortVelement(opt_point.like,opt_point.comment,opt_point.favour,opt_point.forward,score))
        {
            cout << "短视频四要素图像对比通过.."<< endl;
            break;
        }

    }
    if(var>5)
    {
        cout << "短视频四要素图像对比失败.."<< endl;
        return -1;
    }
    // sleep(2);
    return 0;
}

void Thread_Tikok::randomCickScreen()
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

int Thread_Tikok::EntranceLivingRoom(string name)
{
    if (SearchPersonZone(name) == -1)
    {
        cout << "无法搜索 推出寻找直播间程序\n" <<endl;
        return -1;
    }

    double score;
    ad_point match={0};
    ad_point clickP = TIKTOK_OPT_SEARCH;
    ad_point searchClick = {0};
    int var =0;
    for ( var = 0; var < 5; ++var)
    {
        // INPUT_TAP(clickP);
        match = FindTargetForDelay(TIKTOK_SEARCH_LIVING_CV,score,15);
        if(match.x == -1 || match.y == -1)
        {
            cout << "没有找到直播按钮...\n" <<endl;
            continue;
        }
        else
        {
            cout << "点击直播按钮.."  <<match.x<<"," <<match.y<<".\n" <<endl;
            INPUT_TAP(match);
            break;
        }
    }
    if(var>=5)
    {
        cout << "搜索页面打开失败...\n" <<endl;
        return -1;
    }

    for ( var = 0; var < 5; ++var)
    {
        //  INPUT_TAP(clickP);
        match = FindTargetForDelay(TIKTOK_LIVING_UI_CV,score,3);
        if(match.x == -1 || match.y == -1)
        {
            cout << "没有找到直播间...\n" <<endl;
            match = FindTargetForDelay(TIKTOK_LIVING_UI_2_CV,score,3);
            if(match.x == -1 || match.y == -1)
            {
                INPUT_TAP(clickP);
                cout << "点击搜索后重试一次..\n" <<endl;

                continue;
            }

        }
        cout << "进入直播间.."  <<match.x<<"," <<match.y<<".\n" <<endl;
        INPUT_TAP(match);
        // SHORT_DELAY;
        break;
    }

    for (int i = 0; i < 15; ++i)
    {
        cout << "检查直播间三要素 >>>......\n" << endl;

        if( isLivingRoom()>=3)
        {
            cout << "确认完毕 已经进入>>>......\n" << endl;
            break;
        }
        else if(i >=14)
        {
            cout <<"进入直播间失败\n" <<endl;
            return -1;
        }

    }

    return 0;
}
int Thread_Tikok::RandomFollowUser()
{


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
                clickP = {56,464};
                INPUT_TAP(clickP);
                SHORT_DELAY;
                return -1;
            }
            else if(var >=2)
            {
                cout << "一直未发现关注按钮\n" <<endl;
                return -1;
            }
            else
            {

                match = FindTargetReturnPoint(TIKTOK_LIVING_FOLLOWED_2_UI_CV);//todo
                if(match.x>0&& match.y>0)
                {
                    cout << "该用户已经关注\n"<<endl;
                    clickP = {56,464};
                    INPUT_TAP(clickP);
                    SHORT_DELAY;
                    return -1;
                }
            }
            continue;
        }
        else
        {
            clickP = match;
            INPUT_TAP(clickP);
            cout << "已经随机一名用户\n" << endl;
            sleep(1);
            cout << "等待关注结果\n" <<endl;
            for(int var =0 ;var <5 ;var ++)
            {
                ad_point match = FindTargetReturnPoint(TIKTOK_LIVING_FOLLOWED_UI_CV);//
                if(match.x== -1 || match.y== -1)
                {
                    match = FindTargetReturnPoint(TIKTOK_LIVING_PROHIBIT_FOLLOWED_UI_CV);//
                    if(match.x>1 && match.y > 1)
                    {
                        cout << "账号不被允许关注\n" <<endl;
                        return -3;
                    }



                    cout << "未发现关注成功  继续点击关注\n" <<endl;
                    if(var >=4)
                    {
                        cout << "无法关注  可能已达每日上限\n" <<endl;
                        clickP = {56,464};
                        INPUT_TAP(clickP);
                        return -1;
                    }
                    else
                    {
                        INPUT_TAP(clickP);
                    }
                }
                else
                {
                    cout << "关注成功\n" <<endl;
                    LONG_DELAY;
                    clickP = {56,464};
                    INPUT_TAP(clickP);
                    break;
                }
            }
            break;
        }
    }
    return 0 ;
}

int Thread_Tikok::SendBraggerForLivingRoom(string message,bool noEdit)
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
    ad_point clickP =TIKTOK_OPT_BARRAGE_BUTTON;
    ad_point sendMsg  ;
    ad_point match;
    double score;
    int var=0;
    clickP = TIKTOK_OPT_BARRAGE_BUTTON;
    INPUT_TAP(clickP);
    for( var = 0; var < 5; ++var)
    {

        match = FindTargetForDelay(TIKTOK_LIVING_KEYBOARD_UI_CV,score,5);
        if(match.x == -1 || match.y == -1)
        {
            cout << "等待键盘弹出...\n" <<endl;

            clickP = TIKTOK_OPT_BARRAGE_BUTTON;
            INPUT_TAP(clickP);

            continue;
        }
        else
        {
            cout << "键盘弹出成功.."  <<match.x<<"," <<match.y<<".\n" <<endl;
            break;
        }
    }
    if(var>4)
    {
        cout << "键盘弹出失败\n" <<endl;

        return -1;
    }
    for( var = 0; var < 5; ++var)
    {
        match = FindTargetForDelay(TIKTOK_PRESSSEND_LIVINGROOM_CV,score,5);
        if(match.x == -1 || match.y == -1)
        {
            cout << "点击编辑栏目...\n" <<endl;
            continue;
        }
        else
        {
            cout << "已经点击.."  <<match.x<<"," <<match.y<<".\n" <<endl;
            INPUT_TAP_DELAY(match,750);
            clickP = match ;
            sendMsg =match;
            sendMsg.x += 110;
            break;
        }
    }

    for( var = 0; var < 5; ++var)
    {
        match = FindTargetForDelay(TIKTOK_LIVINGROOM_BRAGGER_PASTE_CV,score,5);
        if(match.x == -1 || match.y == -1)
        {
            cout << "无法定位粘贴\n" <<endl;
            INPUT_TAP_DELAY(clickP,750);
            continue;
        }
        else
        {
            cout << "已经点击粘贴.."  <<match.x<<"," <<match.y<<".\n" <<endl;
            INPUT_TAP(match);
            break;
        }
    }


    INPUT_TAP(sendMsg);
    return 0;
}

int Thread_Tikok::FollowSpecifiedUser(string name)
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

int Thread_Tikok::SendMessageToPerson(string name,string message)
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

int Thread_Tikok::isLivingRoom()
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


    match = FindTargetReturnPoint(TIKTOK_LIVING_TERMINATE_UI_CV);//todo
    if(match.x>0&& match.y>0)
    {
        eleGet =-1;
        cout << "直播已经终止\n"<<endl;
    }
    else
        eleGet ++;

    return eleGet;
}
bool Thread_Tikok::LaunchToHomepage()
{
    if(!CheckLaunching())
    {
        return false;
    }
    else
    {
        bool isEnterHomePage =false;
        int var=0;
        for ( var = 0; var < 10; ++var)
        {
            isEnterHomePage =ShowMyHomepage();
            if(isEnterHomePage)
            {
                break;
            }
            CheckFirstLaunch();
            CheckUpgrade();
            if(checklogin())
            {
                isLogin = false;
                return false;
            }
            usleep(500*1000);

        }
        if(var >=10)
        {
            cout << "未能进入主页.....\n"<< endl;
            INPUT_BACK();
            return false;
        }
        else
        {
            cout << "成功进入主页.....\n"<< endl;
            return true;
        }
    }
}
void Thread_Tikok::scrollingUP()
{
    // 给start和end坐标添加随机抖动
    ad_point start = {512 + getRandomInRange(-12, 12), 300 + getRandomInRange(-10, 10)};
    ad_point end = {512 + getRandomInRange(-12, 12), 50 + getRandomInRange(-10, 10)};

    // 给duration添加随机抖动
    int duration = getRandomInRange(450, 650);

    // 执行滑动操作
    INPUT_SWIPE(start, end, duration);
}
void Thread_Tikok::scrollingDown()
{
    // 给start和end坐标添加随机抖动
    ad_point start = {512 + getRandomInRange(-12, 12), 50 + getRandomInRange(-10, 10)};
    ad_point end = {512 + getRandomInRange(-12, 12), 300 + getRandomInRange(-10, 10)};

    // 给duration添加随机抖动
    int duration = getRandomInRange(450, 650);

    // 执行滑动操作
    INPUT_SWIPE(start, end, duration);
}

void Thread_Tikok::selectTaskPreExec()
{
    if (action.sub_action == "私信")
    {
        TASK_EXEC = TASK_SEND_MESSAGE;
    }
    else if (action.sub_action == "直播")
    {
        TASK_EXEC = TASK_LVIVINGROOM_ONSITE;
    }
    else if (action.sub_action == "评论")
    {
        TASK_EXEC = TASK_CONTENT_OPTRATION;
    }
    else if (action.sub_action == "关注")
    {
        TASK_EXEC = TASK_FOLLOW_SOMEONE;
    }
    else if (action.sub_action == "弹幕")
    {
        TASK_EXEC = TASK_LVIVINGROOM_BULLET;
    }
    else if (action.sub_action == "退出")
    {
        TASK_EXEC= TASK_QUIT;
    }
    else if (action.sub_action == "互粉")
    {
        TASK_EXEC = TASK_FOLLOW_MODE;
    }
    else if (action.sub_action == "刷视频")
    {
        TASK_EXEC = TASK_SCROLLING_MODE;
    }
    else
    {
        TASK_EXEC = TASK_UNKNOW;
    }

    cout << "准备启动活动: "<<action.sub_action <<"【" << action.start_time << " - " << action.end_time << "】"<<endl;
}

#if 0

bool ClearFinishedCommand(Dev_Action & action, class APP_TIKTOK &app_tiktok)
{
    if(action.isRunning&& action.action=="抖音"&&app_tiktok.COMMAND ==APP_TIKTOK::ACTING_COMMAND::NONE)
    {//todo
        std::cout << "活动已经提前结束:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        return true;
    }
    if(action.isRunning&& action.action=="抖音"&&action.sub_action=="私信"&&app_tiktok.COMMAND !=APP_TIKTOK::ACTING_COMMAND::SEND_MESSAGE)
    {//todo
        std::cout << "活动已经提前结束:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        return true;
    }
    if(action.isRunning&& action.action=="抖音"&&action.sub_action=="弹幕"&&app_tiktok.COMMAND !=APP_TIKTOK::ACTING_COMMAND::LVIVINGROOM_BULLET)
    {//todo
        std::cout << "活动已经提前结束:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        return true;
    }
    if(action.isRunning&& action.action=="抖音"&&action.sub_action=="评论"&&app_tiktok.COMMAND !=APP_TIKTOK::ACTING_COMMAND::CONTENT_OPTRATION)
    {//todo
        std::cout << "活动已经提前结束:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        return true;
    }
    return false;;
}
#endif
