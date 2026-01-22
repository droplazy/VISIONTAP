#include <unistd.h>
#include "json_utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "mqtt_thread.h"
#include "ThreadBase.h"
#include "Thread_Tikok.h"
#include "http_utils.h"
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <set>
#include <map>




void printSubActions(const vector<Dev_Action>& actions_vector);
void SchedulingProcess(struct Dev_Action *currentAct ,ThreadBase *&p_thread);
int test_download_from_server();
bool initGPIO142();
bool toggleGPIO142();
std::string captureScreenshot();
int ensure_directory_exists(const char *path);
bool setGPIO142(int value);
bool isThirdPartyAppsEmptySimple();
bool installFactoryApps();
bool checkInternetConnection();
bool checkSpecificAppsInstalled(std::vector<std::string>& missingApps);
std::string getApkFilenameForPackage(const std::string& packageName);

int main()
{
    system("echo "" > cv_exe.log");
    initGPIO142();
    setGPIO142(0);

    if(checkInternetConnection())
    {
        setGPIO142(1);
        installFactoryApps();
        getdeviceIp();
        setGPIO142(0);

    }
    cout << "VERSION :" <<VERSION <<endl;
    // ensure_directory_exists("/data/machine_vision/upgradeDir");
    struct Dev_Action *currentAct = nullptr;
    ThreadBase *p_applation = nullptr;
    vector<Dev_Action> actions_vector;
    actions_vector.reserve(24);
    MqttThread mqttClient(IP_HOST, 1883, "username", "password");
    mqttClient.start();
    string mqtt_topic;
    string mqtt_message;
    // struct Dev_Action *currentAct = nullptr;
    bool aaa = false;
    string ProcessID ="";
    auto last_time = std::chrono::steady_clock::now();
    // 主循环：任务调度
    while (1) {
        if(!checkInternetConnection())
        {
            cout << "网络无连接，指示灯快闪1秒" << endl;
            auto start_time = std::chrono::steady_clock::now();
            while (std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now() - start_time).count() < 1000)
            {
                toggleGPIO142();              //
                usleep(50000);
            }
            setGPIO142(0);
        }

        if (mqttClient.isMessageComing(mqtt_message, mqtt_topic)) {
            ParseMqttMassage(mqtt_message, actions_vector);

        }
        // cout << toggleGPIO142() <<endl;
        TraverActionsVector(actions_vector, currentAct);

        if (currentAct != nullptr) {
            toggleGPIO142();
            SchedulingProcess(currentAct, p_applation);
            // 安全退出线程 方法  先等app状态标志为退出成功后在这里 调用safestop 重要
            // cout << "当前活动:" << currentAct->sub_action <<endl;
            if (p_applation != nullptr) {

                if (p_applation->applacationstate == ThreadBase::AppState::EXITING ||
                    p_applation->applacationstate == ThreadBase::AppState::ERROR   ||
                    p_applation->applacationstate == ThreadBase::AppState::NEEDLOGIN   ) {
                    cout << "线程已经退出... "  << endl;
                    string t_ip =IP_HOST;
                    string url ="http://"+t_ip+":8080/dev/upload?commandid="+currentAct->command_id;
                    string picPath =captureScreenshot();
                    Uploadfile(url,picPath);
                    string cmd = "rm "+picPath;
                    system(cmd.c_str());

                    string msg ;
                    if(p_applation->applacationstate == ThreadBase::AppState::ERROR)
                    {
                        msg=createApplicationStatusJson(*currentAct,"failed");
                    }
                    else if(p_applation->applacationstate == ThreadBase::AppState::NEEDLOGIN )//shifou xielou
                    {
                        msg=createApplicationStatusJson(*currentAct,"unlogin");
                    }
                    else
                    {
                        msg=createApplicationStatusJson(*currentAct,"finish");
                    }
                         p_applation->onAppExit();
                         p_applation->safeStop();
                     p_applation= nullptr;
                    mqttClient.pubMessage(msg);
                    currentAct->compeleted = true; // 交给回收接口去处理
                    currentAct = nullptr;
                    // continue;
                } else if (currentAct->sub_action == "弹幕" &&
                           currentAct->action == "抖音") {
                    Thread_Tikok *p_tikok =static_cast<Thread_Tikok *>(p_applation); // TODO 感觉强转失败了
                    if (p_tikok->TASK_EXEC ==Thread_Tikok::TASK_LVIVINGROOM_BULLET_SENT) {
                        currentAct->sub_action = "直播";
                        p_tikok->TASK_EXEC = Thread_Tikok::TASK_LVIVINGROOM_ONSITE;
                        cout << "已经更新状态为直播" << endl;
                    }
                }
            } else {
                cout << "p_applation 指针为空" << endl;
                if (currentAct->compeleted) {
                    //截图上传TODO

                    currentAct = nullptr;
                }
            }

        } else {
            setGPIO142(0);
        }

        pollAndRemoveCompletedActions(actions_vector); // 清除已经结束或者无效的动作
        //  cout << "*********************" <<endl;
        // printSubActions(actions_vector); //打印出所有任务队列
        //  cout << "+++++++++++++++++++++" <<endl;
        // 心跳
        // 获取当前时间
        auto current_time = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(current_time -
                                                             last_time)
                .count() >= 30) {
            string msg = GetdeviceInfoAndresJson(currentAct);
            mqttClient.pubMessage(msg);
            last_time = current_time; // 更新上次调用时间
        }
        //   sleep(1);
    }

    mqttClient.stop();

    return 0;
}

bool isThirdPartyAppsEmptySimple() {
    std::vector<std::string> missingApps;
    return !checkSpecificAppsInstalled(missingApps) && missingApps.size() == 4;
}
bool installFactoryApps() {
    std::cout << "=== 检查出厂应用安装状态 ===" << std::endl;

    // 1. 检查缺少哪些应用
    std::cout << "\n1. 检查应用安装状态..." << std::endl;

    std::vector<std::string> missingApps;
    if (checkSpecificAppsInstalled(missingApps)) {
        std::cout << "✓ 所有出厂应用已安装，无需操作" << std::endl;
        return true;
    }

    std::cout << "发现缺少 " << missingApps.size() << " 个应用:" << std::endl;
    for (size_t i = 0; i < missingApps.size(); i++) {
        std::cout << "  " << (i+1) << ". " << missingApps[i] << std::endl;
    }

    // 显示当前已安装的应用
    std::cout << "\n当前已安装的第三方应用:" << std::endl;
    system("pm list packages -3");

    // 2. 创建下载目录
    std::string download_dir = "/data/tmp/";
    std::string mkdir_cmd = "mkdir -p " + download_dir;
    if (system(mkdir_cmd.c_str()) != 0) {
        std::cout << "✗ 创建下载目录失败" << std::endl;
        return false;
    }

    // 3. 下载并安装缺少的应用
    string t_ip =IP_HOST;

    std::string base_url = "http://"+t_ip+":8080/download?filename=";

    for (size_t i = 0; i < missingApps.size(); i++) {
        const std::string& packageName = missingApps[i];
        std::string apkName = getApkFilenameForPackage(packageName);

        if (apkName.empty()) {
            std::cout << "\n✗ 未知的应用包名: " << packageName << "，跳过" << std::endl;
            continue;
        }

        std::cout << "\n" << (i+1) << "/" << missingApps.size()
                  << " 安装: " << packageName
                  << " (" << apkName << ")" << std::endl;

        std::string filepath = download_dir + apkName;

        // 下载APK
        std::cout << "  下载中..." << std::endl;
        std::string download_url = base_url + apkName;

        if (Downloadfile(download_url, filepath) != 0) {
            std::cout << "  ✗ 下载失败" << std::endl;
            continue;
        }
        std::cout << "  ✓ 下载成功" << std::endl;

        // 检查文件是否存在
        struct stat buffer;
        if (stat(filepath.c_str(), &buffer) != 0) {
            std::cout << "  ✗ 下载文件不存在" << std::endl;
            continue;
        }

        // 安装APK
        std::cout << "  安装中..." << std::endl;
        std::string install_cmd = "pm install -r \"" + filepath + "\" 2>&1";
        FILE* install_pipe = popen(install_cmd.c_str(), "r");
        bool install_success = false;

        if (install_pipe) {
            char output[256];
            while (fgets(output, sizeof(output), install_pipe) != nullptr) {
                std::string line(output);
                // 检查安装成功的关键字
                if (line.find("Success") != std::string::npos ||
                    line.find("success") != std::string::npos) {
                    install_success = true;
                }
                std::cout << "    " << line;
            }
            pclose(install_pipe);
        }

        if (install_success) {
            std::cout << "  ✓ 安装成功" << std::endl;
        } else {
            std::cout << "  ✗ 安装失败" << std::endl;
        }

        // 删除下载的APK文件
        std::string rm_cmd = "rm -f \"" + filepath + "\"";
        system(rm_cmd.c_str());

        // 短暂延迟
        sleep(1);
    }

    // 4. 显示最终安装结果
    std::cout << "\n=== 安装完成 ===" << std::endl;

    // 再次检查安装状态
    std::vector<std::string> stillMissing;
    bool allInstalled = checkSpecificAppsInstalled(stillMissing);

    if (allInstalled) {
        std::cout << "✓ 所有出厂应用已成功安装" << std::endl;
    } else {
        std::cout << "✗ 仍有 " << stillMissing.size() << " 个应用未安装:" << std::endl;
        for (const auto& app : stillMissing) {
            std::cout << "  - " << app << std::endl;
        }
    }

    std::cout << "\n当前已安装的第三方应用:" << std::endl;
    system("pm list packages -3");

    return allInstalled;
}
bool checkInternetConnection();
// 初始化142号GPIO为输出模式
bool initGPIO142() {
    // 导出GPIO
    std::ofstream export_file("/sys/class/gpio/export");
    if (!export_file) return false;
    export_file << "142";
    export_file.close();

    usleep(100000); // 等待100ms，确保导出完成

    // 设置为输出模式
    std::ofstream direction_file("/sys/class/gpio/gpio142/direction");
    if (!direction_file) return false;
    direction_file << "out";
    direction_file.close();

    return true;
}
// 翻转142号GPIO电平
bool toggleGPIO142() {
    // 读取当前值
    std::ifstream value_file("/sys/class/gpio/gpio142/value");
    if (!value_file) return false;

    std::string current_value;
    value_file >> current_value;
    value_file.close();

    // 翻转值
    int new_value = (current_value == "0") ? 1 : 0;

    // 写入新值
    std::ofstream out_file("/sys/class/gpio/gpio142/value");
    if (!out_file) return false;
    out_file << new_value;
    out_file.close();

    return true;
}
std::string captureScreenshot() {
    // 生成时间戳
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", local_time);

    // 构建文件名
    std::string filename = std::string(timestamp) + ".png";
    std::string full_path = "/data/machine_vision/" + filename;

    // 执行截屏命令
    std::string command = "screencap -p " + full_path;
    system(command.c_str());
    //   cout << "snap screen >> " << command.c_str() <<endl;
    // 返回完整文件路径
    return full_path;
}

int ensure_directory_exists(const char *path) {
    struct stat st;

    // 检查目录是否已存在
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            printf("目录已存在: %s\n", path);
            return 0;
        } else {
            fprintf(stderr, "路径存在但不是目录: %s\n", path);
            return -1;
        }
    }

    // 目录不存在，尝试创建
    if (mkdir(path, 0755) == 0) {
        printf("目录创建成功: %s\n", path);
        return 0;
    }

    // 如果创建失败，尝试创建父目录
    fprintf(stderr, "创建目录失败 %s: %s\n", path, strerror(errno));

    return -1;
}
// 控制142号GPIO电平
// 控制142号GPIO电平
bool setGPIO142(int value) {
    if (value != 0 && value != 1) return false;

    std::ofstream out_file("/sys/class/gpio/gpio142/value");
    if (!out_file) return false;

    out_file << value;
    out_file.close();

    return true;
}


void SchedulingProcess(struct Dev_Action *currentAct ,ThreadBase *&p_thread)
{

    if(currentAct->isRunning==false)
    {
        std::cout << "开始活动:"<<currentAct->action << currentAct->sub_action<< std::endl;
        std::cout << "开始时间:"<<currentAct->start_time << "停止时间:"<<currentAct->end_time << std::endl;

        currentAct->print();
        currentAct->isRunning =true;

        if(currentAct->action == "抖音" && p_thread == nullptr)
        {
            p_thread = new Thread_Tikok("tiktok_thread",*currentAct);
            p_thread->start();
        }
        else if(currentAct->action == "抖音" && p_thread != nullptr && \
                 (currentAct->sub_action=="弹幕" || currentAct->sub_action=="退出"|| currentAct->sub_action=="验证码" ))
        {
            p_thread->TaskUpdate(*currentAct);
        }
        else
        {
            std::cout <<"错误发生！"<< std::endl;
            currentAct->compeleted =true;
        }
    }

    if (((compareTime(currentAct->end_time) >= 0 && currentAct->isRunning) && !currentAct->compeleted) || \
        (currentAct->Forcestop && !currentAct->compeleted))
    {

        std::cout << "完成活动:"<<currentAct->action << currentAct->sub_action<< std::endl;
        std::cout << "开始时间:"<<currentAct->start_time << "停止时间:"<<currentAct->end_time << std::endl;

        currentAct->compeleted=true;
        currentAct =nullptr;

        if(p_thread== nullptr)
        {
            cout <<"警告 ：没有发现运行的app管理线程"<<endl;
        }
        else
        {
            //截图上传TODO
            p_thread->safeStop();
            p_thread = nullptr;
        }
        // currentAct.isRunning =false;
    }
    //   cout << "接口结束"<<endl;

}

void printSubActions(const vector<Dev_Action>& actions_vector) {
    for (const auto& action : actions_vector) {
        cout << action.sub_action << endl;  // 打印每个 Dev_Action 的 sub_action
        cout << action.start_time << endl;  // 打印每个 Dev_Action 的 sub_action
        cout << action.end_time << endl;  // 打印每个 Dev_Action 的 sub_action

    }
}

bool checkSpecificAppsInstalled(std::vector<std::string>& missingApps) {
    // 期望安装的应用包名列表
    std::vector<std::string> expectedApps = {
        "com.ss.android.ugc.aweme",     // 抖音
        "tv.danmaku.bili",              // B站
        "com.weico.international",      // 微博国际版
        "com.xingin.xhs"                // 小红书
    };

    // 执行命令获取已安装的第三方应用
    FILE* pipe = popen("pm list packages -3", "r");
    if (!pipe) {
        return false;
    }

    // 读取已安装的应用列表
    std::set<std::string> installedApps;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string line(buffer);
        // 提取包名（去掉"package:"前缀和换行符）
        if (line.find("package:") == 0) {
            std::string pkg = line.substr(8);
            pkg.erase(pkg.find_last_not_of("\n\r") + 1);
            installedApps.insert(pkg);
        }
    }
    pclose(pipe);

    // 检查缺少哪些应用
    missingApps.clear();
    for (const auto& expectedApp : expectedApps) {
        if (installedApps.find(expectedApp) == installedApps.end()) {
            missingApps.push_back(expectedApp);
        }
    }

    return missingApps.empty();
}
// APK文件名映射表
std::string getApkFilenameForPackage(const std::string& packageName) {
    static const std::map<std::string, std::string> apkMapping = {
        {"com.ss.android.ugc.aweme", "aweme_46141150a_v1015_370001_ee02_1765292206.apk"},
        {"tv.danmaku.bili", "tv.danmaku.bili.apk"},
        {"com.weico.international", "com.weico.international.apk"},
        {"com.xingin.xhs", "v9112801-xhs-share_package_common_X64.apk"}
    };

    auto it = apkMapping.find(packageName);
    return (it != apkMapping.end()) ? it->second : "";
}
bool checkInternetConnection() {
    // 在你的设备上，system()返回值可能不可靠
    // 我们直接检查ping的输出

    std::string cmd = "ping -c 2 www.baidu.com 2>&1";
    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        return false;
    }

    char buffer[128];
    std::string output;

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        output += buffer;
    }

    int status = pclose(pipe);

    // 检查输出中是否包含成功标志
    bool success = (output.find("ttl=") != std::string::npos) ||
                   (output.find("time=") != std::string::npos) ||
                   (output.find("bytes from") != std::string::npos);

    if (success) {
        //   std::cout << "✓ 网络连接正常" << std::endl;
        return true;
    }

    std::cout << "✗ 网络连接失败" << std::endl;
    std::cout << "命令输出: " << output.substr(0, 100) << std::endl;
    return false;
}
#if 0

        else if(currentAct->action == "抖音" && p_thread != nullptr &&currentAct->sub_action=="弹幕")
        {
            p_thread->TaskUpdate(*currentAct);
            currentAct->compeleted =true;

        }


        TraverActionsVector(actions_vector,currentAct);

        if (currentAct != nullptr) {
          SchedulingProcess(currentAct);
            std::cout << "当前活动：" << currentAct.sub_action <<  std::endl;
          //  currentActprint();

        } else {
            std::cout << "当前没有活动" << std::endl;
        }
      //
        if(mqtt_msg.newmsg)
        {

        }
#endif
#if 0




#endif

#if 0

    class APP_TIKTOK app_tiktok;
    devicedata.current_action.name=   "待命";
    devicedata.current_action.start_time= "00:00:00";
    devicedata.current_action.end_time=   "00:00:00";

    app_tiktok.start();
    getdeviceInfo();

int http_debug() {
    if (!HttpClient::init()) {
        std::cerr << "Failed to initialize cURL!" << std::endl;
        return 1;
    }

    std::cout << "HTTP Client Test - Accessing Baidu" << std::endl;
    std::cout << "===================================" << std::endl;

    // 创建HTTP客户端
    HttpClient client;

    // 配置 - 禁用SSL验证用于测试
    HttpClient::Config config;
    config.timeout = 15;
    config.follow_redirects = true;
    config.verbose = false;
    config.user_agent = "HttpClient/1.0";
    config.verify_ssl = false; // 禁用SSL证书验证

    try {
        // 测试1: 访问百度首页 (HTTPS)
        std::cout << "\n[Test 1] Accessing Baidu homepage (HTTPS)..." << std::endl;
        auto response = client.get("https://www.baidu.com",
                                   std::map<std::string, std::string>(), config);

        if (response.ok()) {
            std::cout << "✓ Success!" << std::endl;
            std::cout << "  Status: " << response.status_code << std::endl;
            std::cout << "  Size: " << response.body.size() << " bytes" << std::endl;
            std::cout << "  Time: " << response.total_time << " seconds" << std::endl;

            // 简单内容检查
            if (response.body.size() > 1000) {
                std::cout << "  ✓ Received meaningful content" << std::endl;

                // 检查HTML特征
                if (response.body.find("<html") != std::string::npos ||
                    response.body.find("<title") != std::string::npos) {
                    std::cout << "  ✓ HTML content detected" << std::endl;
                }
            }
        } else {
            std::cout << "✗ Failed!" << std::endl;
            std::cout << "  Error: " << response.error << std::endl;
        }

        // 测试2: HTTP访问（应该始终工作）
        std::cout << "\n[Test 2] Accessing Baidu via HTTP..." << std::endl;
        auto http_response = client.get("http://www.baidu.com");

        if (http_response.ok()) {
            std::cout << "✓ HTTP access successful!" << std::endl;
            std::cout << "  Status: " << http_response.status_code << std::endl;
            std::cout << "  Size: " << http_response.body.size() << " bytes"
                      << std::endl;
        } else {
            std::cout << "✗ HTTP access failed: " << http_response.error << std::endl;
        }

        // 测试3: 访问其他不需要SSL验证的站点
        std::cout << "\n[Test 3] Testing with httpbin.org..." << std::endl;
        auto test_response = client.get("http://httpbin.org/get");

        if (test_response.ok()) {
            std::cout << "✓ Success!" << std::endl;
            std::cout << "  Status: " << test_response.status_code << std::endl;

            // 显示部分JSON响应
            size_t preview_len = std::min(test_response.body.size(), (size_t)100);
            std::cout << "  Preview: " << test_response.body.substr(0, preview_len)
                      << "..." << std::endl;
        } else {
            std::cout << "✗ Failed: " << test_response.error << std::endl;
        }

        // 测试4: POST请求
        std::cout << "\n[Test 4] Testing POST request..." << std::endl;
        std::map<std::string, std::string> form_data;
        form_data["test"] = "data";
        form_data["key"] = "value";

        auto post_response = client.post_form("http://httpbin.org/post", form_data);

        if (post_response.ok()) {
            std::cout << "✓ POST successful!" << std::endl;
            std::cout << "  Status: " << post_response.status_code << std::endl;
        } else {
            std::cout << "✗ POST failed: " << post_response.error << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "\nException occurred: " << e.what() << std::endl;
    }

    std::cout << "\n===================================" << std::endl;
    std::cout << "Test completed!" << std::endl;

    // 清理cURL
    HttpClient::cleanup();

    return 0;
}


#endif
