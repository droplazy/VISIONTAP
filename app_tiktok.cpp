#include "app_tiktok.h"
#include <iostream>

APP_TIKTOK::APP_TIKTOK() : running(true) {
    // 构造函数
}

APP_TIKTOK::~APP_TIKTOK() {
    // 确保析构时停止线程
    if (t.joinable()) {
        stop();
    }
}

void APP_TIKTOK::start() {
    // 启动线程
    t = std::thread(&APP_TIKTOK::run, this);  // 创建线程并绑定到成员函数
}

void APP_TIKTOK::stop() {
    // 停止线程
    running = false;
    if (t.joinable()) {
        t.join();  // 等待线程结束
    }
}

void APP_TIKTOK::run() {
    // 线程执行的内容
    while (running) {
        std::cout << "RUNNING" << std::endl;  // 每秒打印一次 RUNNING
        std::this_thread::sleep_for(std::chrono::seconds(1));  // 等待一秒
    }
}
