#ifndef PUB_DEFIEN_H   // 判断是否已经定义过 MACHINE_VISION_H
#define PUB_DEFIEN_H   // 如果没有定义过，则定义它

#include <iostream>
#include <unistd.h>
using namespace std;

#include <vector>
#include <string>

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
#define SHORT_DELAY usleep(1000*1000);
#define LONG_DELAY usleep(3000*1000);
#define APPUI_XY 51
#define APK_TIKTOK_NAME "com.ss.android.ugc.aweme"
#endif
