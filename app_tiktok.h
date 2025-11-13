#ifndef APP_TIKTOK_H
#define APP_TIKTOK_H

#include "pulic_def.h"
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <cstdio>
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
    // 启动线程
    void start();
    // 停止线程
    void stop();

    void ContentExtraction();
private:
    // 线程执行的内容
    void run();
    ContentType contentType =UNKNOW;
    bool running;      // 用来控制线程是否继续运行
    std::thread t;     // 线程对象
};

#endif // APP_TIKTOK_H
