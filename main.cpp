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


int Downloadfile(std::string download_url,std::string save_path);
void printSubActions(const vector<Dev_Action>& actions_vector);
void SchedulingProcess(struct Dev_Action *currentAct ,ThreadBase *&p_thread);
int Uploadfile(const std::string& upload_url, const std::string& file_path);
int test_download_from_server();
// Uploadfile("http://192.168.10.103:8080/dev/upload?serial_number=YDAT250701000007&verifycode=123654&filename=asddsa.png",\
//            "/data/machine_vision/background.png");
//return 0;
//Downloadfile("http://192.168.10.103:8080/download?filename=123.txt","/data/machine_vision/123.txt");
// Uploadfile("http://192.168.10.103:8080/dev/upload?serial_number=YDAT250701000007&verifycode=123654&filename=asddsa.png",\
//            "/data/machine_vision/background.png");
// Downloadfile("http://192.168.10.103:8080/download?filename=vision.tar.gz","/data/machine_vision/123.txt");
// return 0;
//test_download_from_server();
int main() {

    ThreadBase *p_applation = nullptr;
    vector<Dev_Action> actions_vector;
    actions_vector.reserve(24);
    string processId;
    MqttThread mqttClient("192.168.10.103", 1883, "username", "password");
    mqttClient.start();
    string mqtt_topic;
    string mqtt_message;
    struct Dev_Action *currentAct = nullptr;
    bool aaa = false;

    auto last_time = std::chrono::steady_clock::now();
    // 主循环：任务调度
    while (1) {
        if (mqttClient.isMessageComing(mqtt_message, mqtt_topic)) {
            ParseMqttMassage(mqtt_message, actions_vector);
        }

        TraverActionsVector(actions_vector, currentAct);

        if (currentAct != nullptr) {
            SchedulingProcess(currentAct, p_applation);
            // 安全退出线程 方法  先等app状态标志为退出成功后在这里 调用safestop 重要
            // cout << "当前活动:" << currentAct->sub_action <<endl;
            if (p_applation != nullptr) {

                if (p_applation->applacationstate == ThreadBase::AppState::EXITING ||
                    p_applation->applacationstate == ThreadBase::AppState::ERROR) {
                    cout << "线程已经退出... " << endl;
                    //截图上传TODO

                    p_applation->safeStop();
                    p_applation = nullptr;
                    currentAct->compeleted = true; // 交给回收接口去处理
                    currentAct = nullptr;
                    // continue;
                } else if (currentAct->sub_action == "弹幕" &&
                           currentAct->action == "抖音") {
                    Thread_Tikok *p_tikok =
                        static_cast<Thread_Tikok *>(p_applation); // TODO 感觉强转失败了
                    if (p_tikok->TASK_EXEC ==
                        Thread_Tikok::TASK_LVIVINGROOM_BULLET_SENT) {
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
            //  cout << "没有活动" <<endl;
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
                .count() >= 5) {
            string msg = GetdeviceInfoAndresJson(currentAct);
            mqttClient.pubMessage(msg);
            last_time = current_time; // 更新上次调用时间
        }
        sleep(1);
    }

    mqttClient.stop();

    return 0;
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
                 (currentAct->sub_action=="弹幕" || currentAct->sub_action=="退出" ))
        {
            p_thread->TaskUpdate(*currentAct);
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

int Downloadfile(std::string download_url, std::string save_path) {
    if (!HttpClient::init()) {
        std::cerr << "Failed to initialize cURL!" << std::endl;
        return -1;
    }

    std::cout << "=== Downloading File ===" << std::endl;
    std::cout << "URL: " << download_url << std::endl;
    std::cout << "Save to: " << save_path << std::endl;

    // 创建HTTP客户端
    HttpClient client;

    // 配置
    HttpClient::Config config;
    config.timeout = 30;
    config.verbose = true;  // 显示详细日志，便于调试
    config.user_agent = "HttpDownloader/1.0";

    // 使用download方法
    if (client.download(download_url, save_path, config)) {
        std::cout << "✓ File downloaded successfully!" << std::endl;

        // 检查文件大小
        std::ifstream file(save_path, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            std::cout << "  File size: " << file.tellg() << " bytes" << std::endl;
            file.close();
        }

        HttpClient::cleanup();
        return 0; // 成功
    } else {
        std::cout << "✗ Download failed!" << std::endl;

        HttpClient::cleanup();
        return -1; // 失败
    }
}
int Uploadfile(const std::string& upload_url, const std::string& file_path)
{
    std::cout << "=== Uploading File ===" << std::endl;
    std::cout << "URL: " << upload_url << std::endl;
    std::cout << "File: " << file_path << std::endl;

    // 检查文件是否存在
    std::ifstream test_file(file_path.c_str(), std::ios::binary);
    if (!test_file.is_open()) {
        std::cout << "✗ Error: File does not exist or cannot be opened: " << file_path << std::endl;
        return -1;
    }
    test_file.close();

    // 初始化HTTP客户端
    if (!HttpClient::init()) {
        std::cout << "✗ Error: Failed to initialize cURL!" << std::endl;
        return -1;
    }

    HttpClient client;

    // 配置
    HttpClient::Config config;
    config.timeout = 60;  // 上传可能需要更长时间
    config.verbose = true;  // 显示详细日志，便于调试
    config.user_agent = "HttpUploader/1.0";
    config.verify_ssl = false;  // 如果是HTTP而非HTTPS，可以禁用SSL验证

    // 从文件路径中提取文件名
    std::string filename;
    size_t last_slash = file_path.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        filename = file_path.substr(last_slash + 1);
    } else {
        filename = file_path;
    }

    std::cout << "Filename: " << filename << std::endl;

    // 使用upload方法（发送multipart/form-data）
    auto response = client.upload(upload_url, file_path, "file", config);

    if (response.ok()) {
        std::cout << "✓ File uploaded successfully!" << std::endl;
        std::cout << "  Status: " << response.status_code << std::endl;
        std::cout << "  Response: " << response.body << std::endl;
    } else {
        std::cout << "✗ Upload failed!" << std::endl;
        std::cout << "  Error: " << response.error << std::endl;
        std::cout << "  Status: " << response.status_code << std::endl;
        return -1;
    }

    // 清理
    HttpClient::cleanup();

    std::cout << "\n=== Upload Completed ===" << std::endl;
    return 0;

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
int test_download_from_server() {
    if (!HttpClient::init()) {
        std::cerr << "Failed to initialize cURL!" << std::endl;
        return 1;
    }

    std::cout << "=== Testing Download from Local Server ===" << std::endl;

    // 创建HTTP客户端
    HttpClient client;

    // 配置
    HttpClient::Config config;
    config.timeout = 30;
    config.verbose = true;  // 显示详细日志，便于调试
    config.user_agent = "HttpDownloader/1.0";

    // 服务器URL
    std::string download_url = "http://192.168.10.103:8080/download?filename=123.txt";
    std::string save_path = "downloaded_123.txt";

    std::cout << "Server URL: " << download_url << std::endl;
    std::cout << "Save to: " << save_path << std::endl;

    try {
        // 方法1: 使用内置的download方法
        std::cout << "\n[Method 1] Using download() method..." << std::endl;
        if (client.download(download_url, save_path, config)) {
            std::cout << "✓ File downloaded successfully!" << std::endl;

            // 检查文件大小
            std::ifstream file(save_path, std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                std::cout << "  File size: " << file.tellg() << " bytes" << std::endl;
                file.close();
            }
        } else {
            std::cout << "✗ Download failed!" << std::endl;
            //    std::cout << "  Error: " << client.get_last_error() << std::endl;
        }

        // 方法2: 使用GET请求并保存响应
        std::cout << "\n[Method 2] Using GET request and manual save..." << std::endl;
        std::string save_path2 = "downloaded_123_method2.txt";

        auto response = client.get(download_url, std::map<std::string, std::string>(), config);

        if (response.ok()) {
            std::cout << "✓ GET request successful!" << std::endl;
            std::cout << "  Status: " << response.status_code << std::endl;
            std::cout << "  Response size: " << response.body.size() << " bytes" << std::endl;

            // 检查响应头
            if (!response.headers.empty()) {
                std::cout << "  Headers received:" << std::endl;
                for (const auto& [key, value] : response.headers) {
                    if (key == "Content-Type" || key == "Content-Length" ||
                        key == "Content-Disposition") {
                        std::cout << "    " << key << ": " << value << std::endl;
                    }
                }
            }

            // 保存到文件
            std::ofstream out_file(save_path2, std::ios::binary);
            if (out_file.is_open()) {
                out_file.write(response.body.c_str(), response.body.size());
                out_file.close();
                std::cout << "  ✓ Saved to: " << save_path2 << std::endl;
            }

            // 显示文件内容（如果是文本文件且不太大）
            if (response.body.size() < 1000) {
                std::cout << "  Content preview:" << std::endl;
                std::cout << "  " << response.body << std::endl;
            }
        } else {
            std::cout << "✗ GET request failed!" << std::endl;
            std::cout << "  Error: " << response.error << std::endl;
        }

        // 方法3: 测试其他文件下载
        std::cout << "\n[Method 3] Testing other files..." << std::endl;
        std::vector<std::string> test_files = {
            "debug.txt",
            "test.txt",
            "report.pdf",
            "image.jpg"
        };

        for (const auto& filename : test_files) {
            std::string test_url = "http://192.168.10.103:8080/download?filename=" + filename;
            std::string test_save = "downloaded_" + filename;

            std::cout << "  Downloading: " << filename << "..." << std::endl;

            auto test_response = client.get(test_url);
            if (test_response.ok()) {
                std::cout << "    ✓ Success (Status: " << test_response.status_code
                          << ", Size: " << test_response.body.size() << " bytes)" << std::endl;

                // 保存文件
                std::ofstream test_file(test_save, std::ios::binary);
                if (test_file.is_open()) {
                    test_file.write(test_response.body.c_str(), test_response.body.size());
                    test_file.close();
                }
            } else {
                std::cout << "    ✗ Failed: " << test_response.error << std::endl;
            }
        }

        // 方法4: 测试带自定义参数的下载
        std::cout << "\n[Method 4] Testing with additional parameters..." << std::endl;

        // 如果有token认证
        std::string auth_url = "http://192.168.10.103:8080/download?filename=123.txt&token=mysecrettoken";
        auto auth_response = client.get(auth_url);

        if (auth_response.ok()) {
            std::cout << "  ✓ Download with auth successful" << std::endl;
        } else if (auth_response.status_code == 401) {
            std::cout << "  ⚠ Requires authentication (expected)" << std::endl;
        } else {
            std::cout << "  ✗ Error: " << auth_response.error << std::endl;
        }

        // 方法5: 测试错误的文件名
        std::cout << "\n[Method 5] Testing with non-existent file..." << std::endl;
        std::string wrong_url = "http://192.168.10.103:8080/download?filename=nonexistent.txt";
        auto wrong_response = client.get(wrong_url);

        if (wrong_response.ok()) {
            std::cout << "  ✓ File found (unexpected)" << std::endl;
        } else {
            std::cout << "  ⚠ File not found (expected)" << std::endl;
            std::cout << "    Status: " << wrong_response.status_code << std::endl;
            std::cout << "    Error: " << wrong_response.error << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "\nException occurred: " << e.what() << std::endl;
    }

    std::cout << "\n===================================" << std::endl;
    std::cout << "Download test completed!" << std::endl;

    // 清理cURL
    HttpClient::cleanup();

    return 0;
}
