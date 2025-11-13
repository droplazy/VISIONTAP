#ifndef PUB_DEFIEN_H   // 判断是否已经定义过 MACHINE_VISION_H
#define PUB_DEFIEN_H   // 如果没有定义过，则定义它

#include <iostream>
#include <unistd.h>
using namespace std;

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
typedef struct {
    int x;
    int y;
} ad_point;
#define SHORT_DELAY usleep(1000*200);
#define APPUI_XY 51
#define APK_TIKTOK_NAME "com.ss.android.ugc.aweme"
#endif
