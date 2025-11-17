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
#define TIKTOK_OPT_LIKES {994, 199}       // 点赞
#define TIKTOK_OPT_COLLECTIONS {1000, 307}   // 收藏
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
#define TIKTOK_OPT_SEND_MSG {982, 157}         // 发送
#define TIKTOK_OPT_ENTER_LIVE {515, 233}       // 进入直播间
#define TIKTOK_OPT_SEND_DANMU {417, 502}       // 弹幕
#define TIKTOK_OPT_SEND2 {971, 105}            // 发送



#define TIKTOK_COMMENTS_CV        "/data/machine_vision/apppic/comments.png"
#define TIKTOK_PRESSSEND_CV       "/data/machine_vision/apppic/pressSend.png"





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
    // 构造函数和析构函数
    APP_TIKTOK();
    ~APP_TIKTOK();

    bool checkAPKRunning(string apk_name);
    void start();// 启动线程
    void stop();// 停止线程
    void ContentExtraction();

    int SearchPersonZone(string Name);
    int SendComment(string comments);
    int SendMessageToPerson(string comments);

private:
    // 线程执行的内容
    void run();
    ContentType contentType =UNKNOW;
    bool running;      // 用来控制线程是否继续运行
    std::thread t;     // 线程对象
};

#endif // APP_TIKTOK_H
