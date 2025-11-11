#ifndef MACHINE_VISION_H   // 判断是否已经定义过 MACHINE_VISION_H
#define MACHINE_VISION_H   // 如果没有定义过，则定义它

#include "pulic_def.h"


// 声明一个测试函数
void test_function();
void turnon_application(AppType apptype);  // 启动指定的应用

// 启动具体应用的函数声明
void TURN_ON_TIKTOK(void);          // 启动 TikTok
void TURN_ON_WECHAT(void);          // 启动 WeChat
void TURN_ON_REDBOOK(void);         // 启动 RedBook
void TURN_ON_MEITUAN(void);         // 启动 Meituan
void TURN_ON_DAZHONGDIANPING(void); // 启动 大众点评
void TURN_ON_BAIDU(void);           // 启动 Baidu
void TURN_ON_ALIPAY(void);          // 启动 Alipay
void TURN_ON_QQ(void);              // 启动 QQ
void TURN_ON_KUAISHOU(void);        // 启动 Kuaishou
void TURN_ON_BILIBILI(void);        // 启动 Bilibili
#endif // 结束 #ifndef
