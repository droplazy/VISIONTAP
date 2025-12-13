#include "ThreadBase.h"

// ParseResult 构造函数实现
ParseResult::ParseResult() : success(false) {}

ParseResult::ParseResult(bool s, const std::string& d, const std::string& e)
    : success(s), data(d), errorMessage(e) {}

// ThreadBase 构造函数
ThreadBase::ThreadBase(const std::string& name)
    : state_(ThreadState::EXITING)
    , shouldExit_(true)
    , threadName_(name) {}

// 移动构造函数
ThreadBase::ThreadBase(ThreadBase&& other) noexcept
    : state_(other.state_.load())
    , shouldExit_(other.shouldExit_.load())
    , threadName_(std::move(other.threadName_)) {
    // 移动线程所有权
    thread_ = std::move(other.thread_);
}

// 移动赋值运算符
ThreadBase& ThreadBase::operator=(ThreadBase&& other) noexcept {
    if (this != &other) {
        // 停止当前线程
        stop();

        // 移动成员变量
        thread_ = std::move(other.thread_);
        state_ = other.state_.load();
        shouldExit_ = other.shouldExit_.load();
        threadName_ = std::move(other.threadName_);
    }
    return *this;
}

// 析构函数
ThreadBase::~ThreadBase() {
    stop();
}

// 主线程函数
void ThreadBase::threadFunction() {
    // 设置线程状态为启动中
    setState(ThreadState::STARTING);

    // 线程启动虚函数
    if (!onThreadStart()) {
        setState(ThreadState::EXITING);
        return;
    }

    // 设置线程状态为空闲
    setState(ThreadState::IDLE);

    std::cout << getName() << ": Entering main loop" << std::endl;

    // 主循环
    while (!shouldExit_) {
        // 检查是否有任务需要处理
        if (hasTask()) {
            // 执行任务
            setState(ThreadState::BUSY);
            executeTask();
            setState(ThreadState::IDLE);
        } else {
            // 如果没有任务，等待一段时间或直到有通知
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait_for(lock, std::chrono::milliseconds(100),
                           [this]() {
                               return shouldExit_ || hasTask();
                           });//条件变量  超时或者退出或者新任务下达 wait_for会解锁上面的互斥锁
        }

        // 检查是否需要退出
        if (shouldExit_) {
            break;
        }
    }

    // 设置线程状态为退出中
    setState(ThreadState::EXITING);

    // 线程退出虚函数
    onThreadExit();

    std::cout << getName() << ": Main loop exited" << std::endl;
}

// 检查是否有任务（默认实现）
bool ThreadBase::hasTask() {
    return false;
}

// 执行任务（默认实现）
void ThreadBase::executeTask() {
    // 默认实现为空，子类需要重写
}

// 设置线程状态
void ThreadBase::setState(ThreadState newState) {
    state_ = newState;
    onStateChanged(newState);
}

// 启动线程
bool ThreadBase::start() {
    if (isRunning()) {
        std::cout << threadName_ << ": Thread is already running" << std::endl;
        return false;
    }

    shouldExit_ = false;
    // 使用 C++11 兼容的方式创建 unique_ptr
    thread_.reset(new std::thread(&ThreadBase::threadFunction, this));

    std::cout << threadName_ << ": Thread started successfully" << std::endl;
    return true;
}

// 停止线程
void ThreadBase::stop() {
    if (!isRunning()) {
        std::cout << threadName_ << ": Thread is not running" << std::endl;
        return;
    }

    // 设置退出标志
    shouldExit_ = true;
    cond_.notify_all();

    // 等待线程结束
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }

    thread_.reset();
    std::cout << threadName_ << ": Thread stopped" << std::endl;
}

// 安全停止（优雅退出）
void ThreadBase::safeStop() {
    if (!isRunning()) {
        std::cout << threadName_ << ": Thread is not running" << std::endl;
        return;
    }

    // 设置退出标志并通知
    shouldExit_ = true;
    cond_.notify_all();

    // 等待线程处理完当前任务
    std::cout << threadName_ << ": Waiting for current task to complete..." << std::endl;
    while (getState() == ThreadState::BUSY) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // 等待线程结束
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }

    thread_.reset();
    std::cout << threadName_ << ": Thread safely stopped" << std::endl;
}

// 通知线程有任务
void ThreadBase::notify() {
    cond_.notify_one();
}

// 获取线程状态
ThreadState ThreadBase::getState() const {
    return state_;
}

// 获取线程状态字符串
std::string ThreadBase::getStateString() const {
    ThreadState currentState = state_.load();

    // 使用 if-else 替换 switch，因为 enum class 是强类型
    if (currentState == ThreadState::STARTING) return "STARTING";
    else if (currentState == ThreadState::IDLE) return "IDLE";
    else if (currentState == ThreadState::BUSY) return "BUSY";
    else if (currentState == ThreadState::EXITING) return "EXITING";
    else return "UNKNOWN";
}

// 检查线程是否在运行
bool ThreadBase::isRunning() const {
    return thread_ != nullptr && state_ != ThreadState::EXITING;
}

// 获取线程名称
std::string ThreadBase::getName() const {
    return threadName_;
}

// ========== 虚函数默认实现 ==========

// 文本解析虚函数（默认实现）
ParseResult ThreadBase::parseText(const std::string& text) {
    // 默认实现：直接返回成功
    std::cout << threadName_ << ": Default parseText called with: " << text << std::endl;
    return ParseResult(true, text, "");
}

// APP启动虚函数（默认实现）
bool ThreadBase::onAppStart() {
    // 默认实现：返回成功
    std::cout << threadName_ << ": Default onAppStart called" << std::endl;
    return true;
}

// APP退出虚函数（默认实现）
void ThreadBase::onAppExit() {
    // 默认实现
    std::cout << threadName_ << ": Default onAppExit called" << std::endl;
}

// 线程启动虚函数（默认实现）
bool ThreadBase::onThreadStart() {
    // 默认实现：调用APP启动
    std::cout << threadName_ << ": Default onThreadStart called" << std::endl;
    return onAppStart();
}

// 线程退出虚函数（默认实现）
void ThreadBase::onThreadExit() {
    // 默认实现：调用APP退出
    std::cout << threadName_ << ": Default onThreadExit called" << std::endl;
    onAppExit();
}

// 线程状态变化虚函数（默认实现）
void ThreadBase::onStateChanged(ThreadState newState) {
    // 默认实现：输出状态变化
    std::cout << threadName_ << ": State changed to " << getStateString() << std::endl;
}
