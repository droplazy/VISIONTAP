#ifndef APP_TIKTOK_H
#define APP_TIKTOK_H

#include <thread>
#include <chrono>

// 类声明
class APP_TIKTOK {
public:
    // 构造函数和析构函数
    APP_TIKTOK();
    ~APP_TIKTOK();

    // 启动线程
    void start();

    // 停止线程
    void stop();

private:
    // 线程执行的内容
    void run();

    bool running;      // 用来控制线程是否继续运行
    std::thread t;     // 线程对象
};

#endif // APP_TIKTOK_H
