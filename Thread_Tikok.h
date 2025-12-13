#ifndef APP_TIKTOK_H
#define APP_TIKTOK_H

#include "pulic_def.h"
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <regex>
#include <tuple>



class ContentExtractor {
public:
    // 提取ID、LINK、MSG和CMD中的内容
    std::tuple<std::string, std::string, std::string ,std::string> extractContent(const std::string& input) {
        // 查找 ID
        size_t id_start = input.find("ID:") + 3; // 找到 ID: 后面的内容
        size_t id_end = input.find(":ID", id_start); // 找到 ID: 后的结束位置
        std::string id = input.substr(id_start, id_end - id_start);

        // 查找 LINK
        size_t link_start = input.find("LINK:") + 5; // 找到 LINK: 后面的内容
        size_t link_end = input.find(":LINK", link_start); // 找到 LINK: 后的结束位置
        std::string link = input.substr(link_start, link_end - link_start);

        // 查找 MSG
        size_t msg_start = input.find("MSG:") + 4; // 找到 MSG: 后面的内容
        size_t msg_end = input.find(":MSG", msg_start); // 找到 MSG: 后的结束位置
        std::string msg = input.substr(msg_start, msg_end - msg_start);
#if 1
        // 查找 CMD
        size_t remark_start = input.find("MARK:") + 4; // 找到 CMD: 后面的内容
        size_t remark_end = input.find(":MARK", remark_start); // 找到 CMD: 后的结束位置
        std::string remark = input.substr(remark_start, remark_end - remark_start);
#endif
        return std::make_tuple(id, link, msg,remark);
    }
};





#define TIKTOK_OPT_COMMENTS {1000, 249}    // 评论
#define TIKTOK_OPT_CLOSE_COMMENTS {1000, 197}  // 关闭评论
#define TIKTOK_OPT_LIKES {994, 200}       // 点赞
#define TIKTOK_OPT_FARVOUR {1000, 307}   // 收藏
#define TIKTOK_OPT_SEARCH {988, 49}        // 搜索
#define TIKTOK_OPT_SEARCH_RETURN {33, 51}   // 搜索返回
#define TIKTOK_OPT_MESSAGES {593, 518}     // 消息
#define TIKTOK_OPT_HOMEPAGE {310, 519}     // 首页
#define TIKTOK_OPT_FRIENDS {419, 521}      // 朋友
#define TIKTOK_OPT_I {707, 515}            // 我
#define TIKTOK_OPT_SHARES {996, 355}       // 分享
#define TIKTOK_OPT_RECOMMEND {392, 290}    // 推荐
#define TIKTOK_OPT_FOLLOW {996, 167}       // 关注

#define TIKTOK_OPT_SEARCH_2 {994, 52}            // 搜索
#define TIKTOK_OPT_TEXT_INPUT {109, 52}        // 文本输入
#define TIKTOK_OPT_CONTINUE_SEARCH {989, 53}   // 继续搜索
#define TIKTOK_OPT_FIRST_ELEMENT {271, 275}    // 搜索第一要素
#define TIKTOK_OPT_SWIPE_UP {123, 263}         // 上滑
#define TIKTOK_OPT_CLICK_FIRST_WORK {123, 263} // 点击第一作品
#define TIKTOK_OPT_LIKE {996, 219}             // 点赞
#define TIKTOK_OPT_OPEN_COMMENTS {970, 277}    // 打开评论列表
#define TIKTOK_OPT_CLICK_EDIT_BOX_1 {179, 519} // 点击编辑框1
#define TIKTOK_OPT_CLICK_EDIT_BOX_2 {227, 69}  // 点击编辑框2
#define TIKTOK_OPT_SEND {981, 129}             // 发送
#define TIKTOK_OPT_COLLECTION {970, 338}       // 收藏
#define TIKTOK_OPT_FOLLOW_2 {984, 127}           // 关注
#define TIKTOK_OPT_SEND_MESSAGE {583, 508}     // 发私信
#define TIKTOK_OPT_CLICK_DIALOG_1 {403, 508}   // 点击对话框1
#define TIKTOK_OPT_CLICK_DIALOG_2 {227, 159}   // 点击对话框2
#define TIKTOK_OPT_SEND_MSG {982, 157}         // 发送私信
#define TIKTOK_OPT_ENTER_LIVE {515, 233}       // 进入直播间推荐业
#define TIKTOK_OPT_SEND_DANMU {417, 502}       // 弹幕
#define TIKTOK_OPT_SEND2 {971, 105}            // 发送
\
#define TIKTOK_OPT_USER_BUTTON {165, 95}       // 用户按钮
#define TIKTOK_OPT_FOLLOW_BUTTON {740, 170}     // 关注按钮
#define TIKTOK_OPT_ZONE_BUTTON {440, 170}     // 进入空间
#define TIKTOK_OPT_LIVING_BUTTON {300, 170}     // 进入直播间 假如在直播的花
#define TIKTOK_OPT_BARRAGE_BUTTON {65, 502}     // 弹幕钮
#define TIKTOK_OPT_HEART {872, 504}             // 小心心



#define TIKTOK_COMMENTS_CV        "/data/machine_vision/apppic/comments.png"
#define TIKTOK_PRESSSEND_CV       "/data/machine_vision/apppic/pressSend.png"
#define TIKTOK_PRESSSEND_LIVINGROOM_CV       "/data/machine_vision/apppic/pressSend_livingrrom.png"
#define TIKTOK_SEND_MESSAGE_CV    "/data/machine_vision/apppic/sendmsg.png"   // 发私信
#define TIKTOK_MAKEING_CV         "/data/machine_vision/apppic/makeing.png"   // 发私信
#define TIKTOK_SEARCH_USER_CV     "/data/machine_vision/apppic/searchuser.png"
#define TIKTOK_SEARCH_LIVING_CV     "/data/machine_vision/apppic/searchliving.png"
#define TIKTOK_LIVINGROOM_BRAGGER_PASTE_CV     "/data/machine_vision/apppic/braggerpaste.png"

#define TIKTOK_LAUNCHING_CV     "/data/machine_vision/apppic/LaunchingTiktok.png"
#define TIKTOK_LATER_CV     "/data/machine_vision/apppic/Later.png"
#define TIKTOK_HOMEPAGE_CV     "/data/machine_vision/apppic/homepage_ele.png"


#define TIKTOK_LIVING_ROOM_CV     "/data/machine_vision/apppic/islivingroom.png"
#define TIKTOK_LIVING_ROOM_CV_MASK     "/data/machine_vision/apppic/isLivingroom_grey.png"
#define TIKTOK_SEND_MESSAGE_UI_CV    "/data/machine_vision/apppic/pressSend_ui.png"   // 发私信
#define TIKTOK_LIVING_UI_CV    "/data/machine_vision/apppic/isLiving.png"   // 直播中
#define TIKTOK_LIVING_UI_2_CV    "/data/machine_vision/apppic/isLiving_2.png"   // 直播中


#define TIKTOK_LIVING_ELE_1_UI_CV    "/data/machine_vision/apppic/living_ele_1.png"   // 直播三要素
#define TIKTOK_LIVING_ELE_2_UI_CV    "/data/machine_vision/apppic/living_ele_2.png"   // 直播三要素
#define TIKTOK_LIVING_ELE_3_UI_CV    "/data/machine_vision/apppic/living_ele_3.png"   // 直播三要素
#define TIKTOK_LIVING_FOLLOW_UI_CV    "/data/machine_vision/apppic/follow.png"   // 直播关注
#define TIKTOK_LIVING_KEYBOARD_UI_CV    "/data/machine_vision/apppic/keyboard.png"   //
#define TIKTOK_LIVING_TERMINATE_UI_CV    "/data/machine_vision/apppic/livingTerminerd.png"   //
#define TIKTOK_LIVING_FOLLOWED_UI_CV    "/data/machine_vision/apppic/Followed.png"   //
#define TIKTOK_LIVING_FOLLOWED_2_UI_CV    "/data/machine_vision/apppic/Followed_2.png"   //
#define TIKTOK_LIVING_PROHIBIT_FOLLOWED_UI_CV    "/data/machine_vision/apppic/ProhibitFollow.png"   //

#define TIKTOK_LIVING_FOLLOW_OK_UI_CV    "/data/machine_vision/apppic/followsuccess.png"   //
#define TIKTOK_LIVING_FOLLOW_MAXED_UI_CV    "/data/machine_vision/apppic/followmax.png"   //

#define TIKTOK_CONTENT_SHARE_CV    "/data/machine_vision/apppic/contentshare.png"   //
#define TIKTOK_CONTENT_SHAREMORE_CV    "/data/machine_vision/apppic/sharemore.png"   //
#define TIKTOK_CONTENT_MUILTYTALK_CV    "/data/machine_vision/apppic/muiltTalk.png"   //
#define TIKTOK_CONTENT_SPEICYTALK_CV    "/data/machine_vision/apppic/speicymuilty.png"   //
#define TIKTOK_CONTENT_SHARESEND_CV    "/data/machine_vision/apppic/sharesend.png"   //
#define TIKTOK_CONTENT_SPEICYTALK_2_CV    "/data/machine_vision/apppic/speicymuilty_2.png"   //.png
#define TIKTOK_CONTENT_SEARCHPAGE_CV    "/data/machine_vision/apppic/searchpage.png"   //.png
#define TIKTOK_CONTENT_SEARCHPFILLED_CV    "/data/machine_vision/apppic/searchfilled.png"   //.png
#define TIKTOK_CONTENT_SHAREDLINK_CV    "/data/machine_vision/apppic/opensharedcontent.png"   //.png
#define TIKTOK_CONTENT_FIRSTLAUNCH_CV    "/data/machine_vision/apppic/firstlaunch.png"   //.png

// 类声明
class Thread_Tikok {
public:
    // 枚举类型
    enum ContentType {
        UNKNOW,
        LIVE_STREAMING,   // 直播
        SHORT_VIDEO,      // 短视频
        ADVERTISEMENT,    // 广告
        LONG_VIDEO        // 长视频
    };
    enum ACTING_COMMAND {
        NONE,
        LVIVINGROOM_ONSITE,
        CONTENT_OPTRATION,
        LVIVINGROOM_BULLET,
        QUIT,
        SEND_MESSAGE,
        FOLLOW_MODE,
        FOLLOW_MODE_RUNNING,
        SCROLLING_MODE,
        FOLLOW_SOMEONE
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
    // 构造函数和析构函数
    Thread_Tikok();
    ~Thread_Tikok();

    ACTING_COMMAND COMMAND =NONE;
    string remark;
    //养号
    void FollowMode(string FollowText, string roomname, int circleTimes);//找直播间互粉
    void ScrollingShortVideos(int clycles);//刷短视频  穿插分享 点赞 评论
    //指令
    int SpecifyContentOperation(string link, CONTENT_OPT opt, string comment);//评论点赞转发推荐 指定作品
    int SpecifyLivingRoomOnSite(string link);


    bool running=false;
    bool checkAPKRunning(string apk_name);
    void start();// 启动线程
    void stop();// 停止线程


    string act_start_time;   // 开始时间
    string act_end_time;     // 结束时间
private:

    bool ProhibitFollow_b =false;//账号已经被禁止关注

    // 线程执行的内容
    void run();
    void scrollingUP();
    ContentType contentType =UNKNOW;
   // bool running;      // 用来控制线程是否继续运行
    std::thread t;     // 线程对象
    void scrollingDown();
    bool SearchShortVelement(ad_point &like, ad_point &comment, ad_point &farvour, ad_point &forward,double &finalscore);
    bool CheckLaunching();//启动中
    void ContentExtraction();//内容获取
    void CheckUpgrade();//检查升级
    bool ShowMyHomepage();
    void beatBack(int cnt);
    void randomCickScreen();//直播间点赞
    int SearchPersonZone(string Name);//进入搜索内容展示
    int enterSpecifyContent(string content ,ad_operations &opt_point);//进入指定的内容;
    int SendMessageToPerson(string name, string message);//发送私信
    bool isLivingRoom();
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
    void turnoffAPP();
    void CheckFirstLaunch();
    bool checklogin();
    bool isLogin =true;

};

#endif // APP_TIKTOK_H
