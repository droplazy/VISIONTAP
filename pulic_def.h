#ifndef PUB_DEFIEN_H   // 判断是否已经定义过 MACHINE_VISION_H
#define PUB_DEFIEN_H   // 如果没有定义过，则定义它

#include <iostream>
#include <unistd.h>
using namespace std;

typedef enum {
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
} AppType;
typedef struct {
    int x;
    int y;
} ad_point;
#define SHORT_DELAY usleep(1000*200);

#endif
