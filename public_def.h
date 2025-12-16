#ifndef PUB_DEFIEN_H   // 判断是否已经定义过 MACHINE_VISION_H
#define PUB_DEFIEN_H   // 如果没有定义过，则定义它

#include <iostream>
#include <unistd.h>
using namespace std;

#include <vector>
#include <string>

#include <regex>
#include <tuple>
#include <chrono>

// 定义一个结构体来存储网卡信息
struct NetworkInfo {
    string ip;
    string mac;
    unsigned long long totalTraffic; // 下载+上传的流量总和（字节为单位）

    // 添加函数来格式化流量
    string formatTraffic() const {
        double traffic = static_cast<double>(totalTraffic);
        string unit = "B"; // 字节
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "KB";
        }
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "MB";
        }
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "GB";
        }
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "TB";
        }
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "PB";
        }
        return  to_string(traffic) + " " + unit;
    }
};

// 定义结构体
struct Action {
    string name;         // 动作名称
    string start_time;   // 开始时间
    string end_time;     // 结束时间
};
// 定义结构体
struct Dev_Action {
    string action;         // 动作名称
    string sub_action;         // 动作名称
    string start_time;   // 开始时间
    string end_time;     // 结束时间
    string remark;     // 结束时间
    bool isRunning=false;
    bool isCommand=false;
    bool Forcestop =false;
    bool compeleted =false;
    bool quitting =false;

    string processId;     // 结束时间
    // 打印所有成员变量的函数
    void print() const {
        cout << "Action: " << action << endl;
        cout << "Sub Action: " << sub_action << endl;
        cout << "Start Time: " << start_time << endl;
        cout << "End Time: " << end_time << endl;
        cout << "Remark: " << remark << endl;
        cout << "Is Running: " << (isRunning ? "true" : "false") << endl;
        cout << "Is Command: " << (isCommand ? "true" : "false") << endl;
        cout << "Forcestop: " << (Forcestop ? "true" : "false") << endl;
        cout << "Completed: " << (compeleted ? "true" : "false") << endl;
        cout << "Process ID: " << processId << endl;
    }
};
struct DeviceData {
    //  string status;               // 设备状态（在线/离线）
    Action current_action;            // 当前动作
   // Action next_action;               // 下一步动作
    string ip;                   // 设备当前IP地址
    string  temperature;                  // 当前温度
    string boot_time;            // 设备开机时间
    bool error_flag;                  // 异常标志
    string warning_message;      // 警告信息
    string totalTraffic;      // 警告信息
    string mac;      // 警告信息
    string serial_number;        // 用户编号（设备序列号）
    string verification_code;    // 验证码
    string usedProcess;
    string ProcessID;

    //  string runtime;
};

struct HeartbeatMessage {
    string timestamp;            // 时间戳
    string messageType;          // 消息类型
    DeviceData data;                  // 设备数据
};

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



class StringList {
public:
    // 添加元素到末尾
    void append(const std::string& str) {
        list.push_back(str);
    }

    // 获取指定位置的元素
    std::string at(int index) const {
        if (index >= 0 && index < list.size()) {
            return list[index];
        }
        return "";  // 返回空字符串，如果索引无效
    }

    // 获取列表的大小
    size_t size() const {
        return list.size();
    }

    // 输出所有字符串
    void print() const {
        for (const auto& str : list) {
            std::cout << str << std::endl;
        }
    }

private:
    std::vector<std::string> list;
};

typedef enum {
    APP_TIKTOK_ENUM = 1,          // TikTok
    APP_WECHAT_ENUM = 2,          // WeChat
    APP_REDBOOK_ENUM = 3,         // RedBook
    APP_MEITUAN_ENUM = 4,         // Meituan
    APP_DAZHONGDIANPING_ENUM = 5, // 大众点评
    APP_BAIDU_ENUM = 6,           // Baidu
    APP_ALIPAY_ENUM = 7,          // Alipay
    APP_QQ_ENUM = 8,              // QQ
    APP_KUAISHOU_ENUM = 9,        // 快手
    APP_BILIBILI_ENUM = 10        // 哔哩哔哩
} AppType;

typedef enum {
    IDLE =0,
    APP_TIKTOK = 1,          // TikTok
    APP_WECHAT = 2,          // WeChat
    APP_REDBOOK = 3,         // RedBook
    APP_MEITUAN = 4,         // Meituan
    APP_DAZHONGDIANPING = 5, // 大众点评
    APP_BAIDU = 6,           // Baidu
    APP_ALIPAY = 7,          // Alipay
    APP_QQ = 8,              // QQ
    APP_KUAISHOU = 9,        // 快手
    APP_BILIBILI = 10        // 哔哩哔哩
} RUNNUNG_APP;


typedef struct {
    int x;
    int y;
} ad_point;

#define STANDBY_ACT "待命"
#define TIKTOK_ACT "抖音"

#define TIKTOK_SUBACT_BURG "弹幕"


#define SHORT_DELAY usleep(1000*1000);
#define LONG_DELAY usleep(3000*1000);
#define APPUI_XY 51
#define APK_TIKTOK_NAME "com.ss.android.ugc.aweme"










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
#define TIKTOK_APPATTASK_CV    "/data/machine_vision/apppic/tiktok_task.png"   // 直播中


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
#define TIKTOK_CONTENT_FIRSTLAUNCH_CV    "/data/machine_vision/apppic/firstlaunch.png"   //.pngcannotopenliving.png
#define TIKTOK_CONTENT_LIVINGNOTSTART_CV    "/data/machine_vision/apppic/cannotopenliving.png"   //.png




















#endif
