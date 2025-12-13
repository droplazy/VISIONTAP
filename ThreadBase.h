#ifndef THREADBASE_H
#define THREADBASE_H

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <vector>
#include <chrono>
#include "public_def.h"
#include "json_utils.h"


// 线程状态枚举
enum class ThreadState {
    STARTING,   // 启动中
    IDLE,       // 空闲
    BUSY,       // 忙碌
    EXITING,     // 退出中
    ERROR    // 退出中
};

// 文本解析结果结构体
struct ParseResult {
    bool success;
    std::string data;
    std::string errorMessage;

    ParseResult();
    ParseResult(bool s, const std::string& d = "", const std::string& e = "");
};

class ThreadBase {
protected:
    std::unique_ptr<std::thread> thread_;           // 线程对象
    std::atomic<ThreadState> state_;                // 线程状态
    std::mutex mutex_;                              // 互斥锁
    std::condition_variable cond_;                  // 条件变量
    std::atomic<bool> shouldExit_;                  // 退出标志
    std::string threadName_;                        // 线程名称

    // 主线程函数
    void threadFunction();

    // 检查是否有任务（子类可重写）
    virtual bool hasTask();

    // 执行任务（子类可重写）
    virtual void executeTask();

    // 设置线程状态
    void setState(ThreadState newState);

public:

    // 线程状态枚举
    enum class AppState {
        STARTING,   // 启动中
        IDLE,       // 空闲
        BUSY,       // 忙碌
        EXITING,     // 退出中
        ERROR    // 退出中
    };


    // 构造函数
    // 必须传入action
    ThreadBase(const std::string &name, Dev_Action devinfo);
    // 虚析构函数
    virtual ~ThreadBase();

    // 禁止拷贝和赋值
    ThreadBase(const ThreadBase&) = delete;
    ThreadBase& operator=(const ThreadBase&) = delete;

    // 移动构造函数和赋值运算符
    ThreadBase(ThreadBase&& other) noexcept;
    ThreadBase& operator=(ThreadBase&& other) noexcept;

    // 线程控制方法
    bool start();
    void stop();
    void safeStop();
    void notify();

    // 状态查询方法
    ThreadState getState() const;
    std::string getStateString() const;
    bool isRunning() const;
    std::string getName() const;

    Dev_Action action;
    AppState applacationstate;
    virtual void TaskUpdate(Dev_Action task);
   // virtual void disposeSubTask();
    // ========== 虚函数（子类需要实现或重写）==========

    // 文本解析虚函数
    virtual void parseText(const std::string& text);

    // APP启动虚函数
    virtual bool onAppStart();

    // APP退出虚函数
    virtual void onAppExit();

    // 线程启动虚函数
    virtual bool onThreadStart();

    // 线程退出虚函数
    virtual void onThreadExit();

    // 线程状态变化虚函数
    virtual void onStateChanged(ThreadState newState);
};

#endif // THREADBASE_H
