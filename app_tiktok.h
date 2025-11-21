#ifndef APP_TIKTOK_H
#define APP_TIKTOK_H

#include "pulic_def.h"
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cstdio>





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
#define TIKTOK_LIVING_KEYBOARD_UI_CV    "/data/machine_vision/apppic/keyboard.png"   // 直播关注
#define TIKTOK_LIVING_TERMINATE_UI_CV    "/data/machine_vision/apppic/livingTerminerd.png"   // 直播已经结束
#define TIKTOK_LIVING_FOLLOWED_UI_CV    "/data/machine_vision/apppic/Followed.png"   // 直播已经结束


// 类声明
class APP_TIKTOK {
public:
    // 枚举类型
    enum ContentType {
        UNKNOW,
        LIVE_STREAMING,   // 直播
        SHORT_VIDEO,      // 短视频
        ADVERTISEMENT,    // 广告
        LONG_VIDEO        // 长视频
    };
    // 枚举类型
    enum CONTENT_OPT {
        GIVELIKE_OPT,
        COMMENT_OPT,
        FAVOURITE_OPT,
        FORWARD_OPT
    };
    // 构造函数和析构函数
    APP_TIKTOK();
    ~APP_TIKTOK();



    //养号
    void FollowMode(string FollowText, int circleTimes);//找直播间互粉
    void ScrollingShortVideos(int clycles);//刷短视频  穿插分享 点赞 评论




    bool checkAPKRunning(string apk_name);
    void start();// 启动线程
    void stop();// 停止线程



private:
    // 线程执行的内容
    void run();
    void scrollingUP();
    ContentType contentType =UNKNOW;
    bool running;      // 用来控制线程是否继续运行
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
    int SendComment(string comments);//发送评论
    int VideoContentLike(string name,string message);//作品点赞  在用户个人主页内 未完成
    void VideoContentOPT(CONTENT_OPT operat);//作品点赞  在用户个人主页内 未完成
    bool VideoContentForward();
    int SendMessageToPerson(string name, string message);//发送私信
    bool isLivingRoom();
    int EntranceLivingRoom(string name);//进入指定直播间
    int SendBraggerForLivingRoom(string message, bool noEdit);//发送弹幕僚
    int FollowSpecifiedUser(string name);//关注指定人
    void RandomFollowUser();//随机关注路人 （在胡粉直播间）
    bool LaunchToHomepage();
};

#endif // APP_TIKTOK_H
