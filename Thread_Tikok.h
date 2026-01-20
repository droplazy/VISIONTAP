#ifndef APP_TIKTOK_H
#define APP_TIKTOK_H

#include "public_def.h"
#include <thread>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "ThreadBase.h"


// 类声明
class Thread_Tikok :public  ThreadBase {
public:

    enum EXECUTE_TASK {
        TASK_NONE,
        TASK_UNKNOW,
        TASK_LVIVINGROOM_ONSITE,
        TASK_CONTENT_OPTRATION,
        TASK_LVIVINGROOM_BULLET,
        TASK_LVIVINGROOM_BULLET_SENT,
        TASK_QUIT,
        TASK_SEND_MESSAGE,
        TASK_FOLLOW_MODE,
        TASK_FOLLOW_SOMEONE,
        TASK_FOLLOW_MODE_RUNNING,
        TASK_SCROLLING_MODE,
        TASK_COMPELTED,
        TASK_EXEC_FAILD,
        TASK_LOGIN,
        TASK_LOGIN_CRCODE,
        TASK_LOGIN_VERIFY_CODE,
    };
    typedef int CONTENT_OPT;
    enum  {
        GIVELIKE_OPT   = 1 << 0,  // 0001
        COMMENT_OPT    = 1 << 1,  // 0010
        FAVOURITE_OPT  = 1 << 2,  // 0100
        FORWARD_OPT    = 1 << 3   // 1000
    };
    typedef struct {
        ad_point like;
        ad_point comment;
        ad_point favour;
        ad_point forward;
    } ad_operations;

    Thread_Tikok(const std::string &name, const Dev_Action &action);
    ~Thread_Tikok();

    EXECUTE_TASK TASK_EXEC =TASK_NONE;
  //  string remark;
    //养号
    void FollowMode(string FollowText, string roomname, int circleTimes);//找直播间互粉
    void ScrollingShortVideos(int clycles);//刷短视频  穿插分享 点赞 评论
    //指令
    int SpecifyContentOperation(string link, CONTENT_OPT opt, string comment);//评论点赞转发推荐 指定作品
    int SpecifyLivingRoomOnSite(string link);

    void TaskUpdate(Dev_Action task) override;

#if 0
    bool checkAPKRunning(string apk_name);
    void start();// 启动线程
    void stop();// 停止线程
#endif


    virtual void parseText(const std::string& text) override;
    virtual bool onAppStart() override;
    virtual void onAppExit() override;
    virtual void onStateChanged(ThreadState newState) override;


protected:
    virtual bool hasTask() override;
    virtual void executeTask() override;

private:

    bool ProhibitFollow_b =false;//账号已经被禁止关注
    bool isBusy =false;

    //备注内容
    std::string commandid;

    std::string remark_id;
    std::string remark_link;
    std::string remark_msg;
    std::string remark_mark;
    // 线程执行的内容
    void run();
    void scrollingUP();
   // bool running;      // 用来控制线程是否继续运行
    std::thread t;     // 线程对象
    void scrollingDown();
    void selectTaskPreExec();
    bool SearchShortVelement(ad_point &like, ad_point &comment, ad_point &farvour, ad_point &forward,double &finalscore);
    bool CheckLaunching();//启动中
    void ContentExtraction();//内容获取
    void CheckUpgrade();//检查升级
    bool ShowMyHomepage();
    void beatBack(int cnt);
    void randomCickScreen();//直播间点赞
    int SearchPersonZone(string Name);//进入搜索内容展示
    int enterSpecifyContent(string content ,ad_operations &opt_point);//进入指定的内容;
    int isLivingRoom();
    int EntranceLivingRoom(string name);//进入指定直播间
    int SendBraggerForLivingRoom(string message, bool noEdit);//发送弹幕僚
    int FollowSpecifiedUser(string name);//关注指定人
    int RandomFollowUser();//随机关注路人 （在胡粉直播间）
    bool LaunchToHomepage();
    void RandomShortVideoOperation(ad_point click, CONTENT_OPT opt,string content);
    void ContentComment(string content);
    void ContentForward();
    int enterSpecifyLivingrom(string content);
    std::tuple<std::string, std::string, std::string> extractContent(const std::string &input);
    void CheckFirstLaunch();
    bool checklogin();
    int SendMessageToPerson(string name, string message);//发送私信

    bool app_login();
    void CheckReminder();

    bool app_login_sel();
    bool app_login_2();
    bool captureLoginCRcode_2();
    bool captureLoginCRcode();
    void checklvingroomwid();
};

#endif // APP_TIKTOK_H
