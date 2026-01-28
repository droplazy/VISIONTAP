#include "json_utils.h"
#include <sstream>

#include <iomanip>
#include <fstream>
#include <regex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cstdio>   // for popen
#include <cstdlib>  // for exit
#include "md5.h"
#include <iostream>
#include "http_utils.h"
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
string processId = "";
string dev_IP= "";
std::string getdeviceIp() {
    // 创建 HttpClient 实例
    HttpClient client;

    // 准备参数
    std::map<std::string, std::string> params;
    params["token"] = VERIFY_CODE;  // 固定参数

    try {
        // 发送 GET 请求
        string t_ip =IP_HOST;

        HttpClient::Response response = client.get(
            "http://"+t_ip+":8080/device/iptest",
            params
            );

        // 检查请求是否成功
        if (!response.ok()) {
            std::cerr << "HTTP 请求失败，状态码: " << response.status_code
                      << "，错误: " << response.error << std::endl;
            return "";
        }

        // 输出原始响应用于调试
        // std::cout << "原始响应: " << response.body << std::endl;

        // 简单解析 JSON，提取 IP 地址
        size_t code_pos = response.body.find("\"code\":");
        size_t message_pos = response.body.find("\"message\":");

        // 检查响应格式
        if (message_pos == std::string::npos) {
            std::cerr << "响应格式不正确" << std::endl;
            return "";
        }

        // 提取 IP 地址部分
        size_t quote_start = response.body.find('\"', message_pos + 10);  // "message": 有10个字符
        if (quote_start == std::string::npos) {
            std::cerr << "无法找到 IP 地址起始位置" << std::endl;
            return "";
        }

        size_t quote_end = response.body.find('\"', quote_start + 1);
        if (quote_end == std::string::npos) {
            std::cerr << "无法找到 IP 地址结束位置" << std::endl;
            return "";
        }

        // 提取 IP 地址
        std::string ip_address = response.body.substr(quote_start + 1, quote_end - quote_start - 1);
        dev_IP = ip_address;
        return ip_address;

    } catch (const std::exception& e) {
        std::cerr << "请求异常: " << e.what() << std::endl;
        return "";
    }
}

// 生成当前时间的辅助函数
string getCurrentTime() {
    time_t now =  time(0);
    tm* tm_now =  localtime(&now);
    stringstream ss;
    ss <<  put_time(tm_now, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}
// 生成心跳包的 JSON 字符串
std::string generateHeartbeatJson(const HeartbeatMessage& heartbeat) {
    std::stringstream json;

    // 获取当前时间戳
    std::string timestamp = heartbeat.timestamp.empty() ? getCurrentTime() : heartbeat.timestamp;

    // 生成 JSON 字符串
    json << "{\n"
         << "  \"timestamp\": \"" << timestamp << "\",\n"
         << "  \"serial_number\": \"" << heartbeat.data.serial_number << "\",\n"
         << "  \"verification_code\": \"" << heartbeat.data.verification_code << "\",\n"
         << "  \"messageType\": \"" << heartbeat.messageType << "\",\n"
         << "  \"data\": {\n"
         << "    \"current_action\": {\n"
         << "      \"name\": \"" << heartbeat.data.current_action.name << "\",\n"
         << "      \"start_time\": \"" << heartbeat.data.current_action.start_time << "\",\n"
         << "      \"end_time\": \"" << heartbeat.data.current_action.end_time << "\"\n"
         << "    },\n"
#if 0
         << "    \"next_action\": {\n"
         << "      \"name\": \"" << heartbeat.data.next_action.name << "\",\n"
         << "      \"start_time\": \"" << heartbeat.data.next_action.start_time << "\",\n"
         << "      \"end_time\": \"" << heartbeat.data.next_action.end_time << "\"\n"
         << "    },\n"
#endif
         << "    \"ip\": \"" << heartbeat.data.ip << "\",\n"
         << "    \"temperature\": " << heartbeat.data.temperature << ",\n"
         << "    \"boot_time\": \"" << heartbeat.data.boot_time << "\",\n"
         << "    \"error_flag\": " << (heartbeat.data.error_flag ? "true" : "false") << ",\n"
         << "    \"warning_message\": \"" << heartbeat.data.warning_message << "\",\n"
         << "    \"firmware_version\": \"" << VERSION << "\",\n"

         << "    \"mac\": \"" << heartbeat.data.mac << "\",\n"
       //  << "    \"runtime\": \"" << heartbeat.data.runtime << "\"\n"    devicedata.ProcessID = "P001";
         << "    \"traffic_statistics\": \"" << heartbeat.data.totalTraffic << "\",\n"
     //./    << "    \"usedProcess\": \"" << heartbeat.data.usedProcess << "\",\n"
         << "    \"ProcessID\": \"" << heartbeat.data.ProcessID << "\"\n"

         << "  }\n"
         << "}";

    return json.str();
}
// 解析 ro.boot.serialno 的函数
std::string parseSerialNumber() {
    // 使用 execCommand 执行 getprop | grep serial 命令
    std::string output = execCommand("getprop | grep serial");

    // 使用正则表达式提取 ro.boot.serialno
    std::regex serialnoRegex(R"(\[ro\.boot\.serialno\]:\s*\[(\S+)\])");
    std::smatch match;

    if (std::regex_search(output, match, serialnoRegex)) {
        return match[1]; // 返回匹配到的序列号
    }

    return "Serial number not found";
}
std::string getUptime() {
    // 使用 execCommand 执行命令并获取输出
    std::string uptime_str = execCommand("cat /proc/uptime");

    // 打印输出以检查是否获取到有效的结果
    //std::cout << "Uptime command output: " << uptime_str << std::endl;

    // 检查命令执行后的输出是否为空
    if (uptime_str.empty()) {
        std::cerr << "Error: No output from 'cat /proc/uptime' command!" << std::endl;
        return "";
    }

    // 获取 uptime 的第一个值，表示开机累计时长（秒）
    std::istringstream iss(uptime_str);
    std::string uptime_seconds;
    iss >> uptime_seconds; // 读取第一个值（秒）

    // 打印解析结果，检查是否成功提取 uptime_seconds
 //   std::cout << "Parsed uptime seconds: " << uptime_seconds << std::endl;

    // 如果没有获取到uptime_seconds，说明输出有问题
    if (uptime_seconds.empty()) {
        std::cerr << "Failed to parse uptime value!" << std::endl;
        return "";
    }

    return uptime_seconds;
}

std::string readTemperature()
{
    std::ifstream tempFile("/sys/class/thermal/thermal_zone0/temp");
    if (!tempFile.is_open()) {
        return "无法读取温度数据!";
    }

    int temp;
    tempFile >> temp;
    tempFile.close();

    // 将温度转化为摄氏度并返回
    double celsiusTemp = temp / 1000.0;
    return std::to_string(celsiusTemp);
}
std::string execCommand(const std::string& cmd) {
    // 使用popen执行命令，并通过管道读取输出
    char buffer[128];
    std::string result = "";

    // 打开管道，执行命令
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen failed!");
    }

    // 逐行读取命令输出
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    // 关闭管道
    fclose(pipe);

    return result;
}// for exit
std::string getUtcTimestamp()
{
    using namespace std::chrono;

    // 获取当前UTC时间点
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);

        t += 8 * 3600;
    // 转成 UTC tm
    std::tm tm_utc{};
#if defined(_WIN32)
    gmtime_s(&tm_utc, &t);
#else
    gmtime_r(&t, &tm_utc);
#endif

    // 格式化成 ISO8601: YYYY-MM-DDTHH:MM:SSZ
    std::ostringstream oss;
    oss << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}
NetworkInfo getNetworkInfo(const std::string& interface) {
    NetworkInfo info;

    // 执行 ifconfig 命令
    std::string cmd = "ifconfig " + interface;
    std::string output = execCommand(cmd);
   // std::cout << "output : " << output << std::endl;

    // 使用正则表达式解析输出，提取 IP 和 MAC 地址
    std::regex ipRegex(R"(inet addr:(\d+\.\d+\.\d+\.\d+))");
    std::regex macRegex(R"(HWaddr\s([0-9a-fA-F]{2}(:[0-9a-fA-F]{2}){5}))");
    std::regex rxBytesRegex(R"((RX\s+bytes:)(\d+))"); // 接收字节数
    std::regex txBytesRegex(R"((TX\s+bytes:)(\d+))"); // 发送字节数

    std::smatch match;

    // 提取 IP 地址
    if (std::regex_search(output, match, ipRegex)) {
        info.ip = match[1];
    }

    // 提取 MAC 地址
    if (std::regex_search(output, match, macRegex)) {
        info.mac = match[1];
    }

    // 提取接收流量
    unsigned long long totalRxBytes = 0, totalTxBytes = 0;
    if (std::regex_search(output, match, rxBytesRegex)) {
        totalRxBytes = std::stoull(match[2].str());
    }

    // 提取发送流量
    if (std::regex_search(output, match, txBytesRegex)) {
        totalTxBytes = std::stoull(match[2].str());
    }

    // 计算总流量（上传 + 下载）
    info.totalTraffic = totalRxBytes + totalTxBytes;

    // // 打印结果
    // std::cout << "totalTraffic : " << info.totalTraffic << std::endl;
    // std::cout << "mac : " << info.mac << std::endl;
    // std::cout << "ip : " << info.ip << std::endl;

    return info;
}

string check_message_type(const string& json_data) {
    // 查找 "messageType" 字段的位置
    size_t pos = json_data.find("\"messageType\":");

    // 如果没有找到字段，则返回空
    if (pos == string::npos) {
   //     cout << "messageType not found!" << endl;
        return "";
    }
  //  cout << "Found \"messageType\": at position " << pos << endl;

    // 从字段位置开始，提取 "messageType" 的值
    pos = json_data.find(":", pos);
    if (pos == string::npos) {
   //     cout << "Colon (:) not found after \"messageType\"" << endl;
        return "";
    }
  //  cout << "Found colon (:) at position " << pos << endl;

    // 跳过冒号，找到第一个非空字符（去掉空格和引号）
    pos = json_data.find_first_not_of(" \t\n\r", pos + 1);
    if (pos == string::npos) {
   //     cout << "No non-whitespace character found after colon" << endl;
        return "";
    }
  //  cout << "Found non-whitespace character at position " << pos << endl;

    // 查找字段的结束位置，假设是双引号或其他标记
    size_t end_pos = json_data.find_first_of("\"", pos + 1);
    if (end_pos == string::npos) {
        cout << "End quote for \"messageType\" not found!" << endl;
        return "";
    }
 //   cout << "Found end quote at position " << end_pos << endl;

    // 提取字符串，返回 messageType 的值
    string message_type = json_data.substr(pos + 1, end_pos - pos - 1);
  //  cout << "Extracted messageType: " << message_type << endl;

    // 返回提取到的 messageType 字段值
    return message_type;
}
// 函数：从 JSON 字符串中提取字段值
string extract_json_field(const string& json_data, const string& field_name) {
    size_t pos = json_data.find("\"" + field_name + "\":");
    if (pos == string::npos) {
        return "";
    }
    pos = json_data.find(":", pos);
    if (pos == string::npos) {
        return "";
    }
    pos = json_data.find_first_not_of(" \t\n\r", pos + 1);
    size_t end_pos = json_data.find_first_of("\"", pos + 1);
    if (end_pos == string::npos) {
        return "";
    }
    return json_data.substr(pos + 1, end_pos - pos - 1);
}
std::string GetdeviceInfoAndresJson(Dev_Action *cur)//TODO
{
    DeviceData devicedata;
    NetworkInfo netinfo =getNetworkInfo("eth0");
    devicedata.ip = dev_IP;//netinfo.ip;
    devicedata.mac = netinfo.mac;
    devicedata.totalTraffic = netinfo.formatTraffic();
    devicedata.error_flag= 0;
    devicedata.temperature= readTemperature();
    devicedata.serial_number = parseSerialNumber();
    devicedata.verification_code = VERIFY_CODE;
    devicedata.boot_time = getUptime();
  //  devicedata.usedProcess = "xxxxx";
    devicedata.ProcessID = processId;
    HeartbeatMessage heartbeat;
    heartbeat.timestamp =getUtcTimestamp();
    heartbeat.messageType="heart";
    if(cur != nullptr)
    {
    devicedata.current_action.name = cur->action;
    devicedata.current_action.start_time = cur->start_time;
    devicedata.current_action.end_time = cur->end_time;
    }
    else
    {
        devicedata.current_action.name = "空闲";
        devicedata.current_action.start_time = "00:00:00";
        devicedata.current_action.end_time = "00:00:00";

    }
    heartbeat.data =devicedata;

    return generateHeartbeatJson(heartbeat);
}
int compareTime(const std::string& target)
{
    auto now = std::chrono::system_clock::now();
    auto utc = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::gmtime(&utc);

    tm.tm_hour += 8;  // UTC+8 = 北京时间
    std::mktime(&tm);

    int curSec = tm.tm_hour*3600 + tm.tm_min*60 + tm.tm_sec;

    int h, m, s;
    char c;
    std::stringstream(target) >> h >> c >> m >> c >> s;

    return curSec - (h*3600 + m*60 + s);
}
void StopAppActivety(class APP_TIKTOK &app, Dev_Action &action)
{
    int retCode = 0; // 0=未处理，1=成功，负值=错误

}

// 简单的提取字符串函数
string extractString(const string& json, const string& key) {
    size_t keyPos = json.find("\"" + key + "\":");
    if (keyPos == string::npos) return ""; // 如果没找到，返回空字符串

    size_t start = json.find("\"", keyPos + key.size() + 3); // 跳过 `key": " 的部分
    if (start == string::npos) return "";

    size_t end = json.find("\"", start + 1); // 找到结束的引号位置
    if (end == string::npos) return "";

    return json.substr(start + 1, end - start - 1); // 提取字符串
}

// 解析 data 数组中的每个对象
void parseDataArray(const string& json, vector<Dev_Action>& actions) {
    // 提取 processId
    processId = extractString(json, "processId");
    cout << "get processid = " <<processId;
    size_t pos = json.find("\"data\":");
    if (pos == string::npos) return; // 没有找到 data 数组

    size_t arrayStart = json.find("[", pos); // 查找 data 数组的起始位置
    size_t arrayEnd = json.find("]", arrayStart); // 查找 data 数组的结束位置
    if (arrayStart == string::npos || arrayEnd == string::npos) return;

    string dataArray = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1); // 提取 data 数组

    size_t objStart = 0;
    while ((objStart = dataArray.find("{", objStart)) != string::npos) { // 遍历每个对象
        size_t objEnd = dataArray.find("}", objStart); // 查找对象的结束位置
        if (objEnd == string::npos) break;

        string obj = dataArray.substr(objStart, objEnd - objStart + 1); // 提取当前对象

        // 将当前对象的字段填充到 Dev_Action 结构体
        Dev_Action action;
        action.action = extractString(obj, "action");
        action.sub_action = extractString(obj, "sub_action");
        action.start_time = extractString(obj, "start_time");
        action.end_time = extractString(obj, "end_time");
        action.remark = extractString(obj, "remark");
        action.processId = processId;
        action.isRunning=false;
        action.isCommand = false;
        action.Forcestop = false;
        action.compeleted  = false;
        actions.push_back(action); // 将填充好的 action 对象加入到容器中

        objStart = objEnd + 1; // 继续查找下一个对象
    }
}
static pid_t audioPid = -1;

bool startAudioStream(const std::string &rtspUrl)
{
    // 如果已经在运行，先停止
    if (audioPid > 0) {
        std::cout << "音频流已经在运行，先停止之前的进程" << std::endl;
        stopAudioStream();
        sleep(1); // 等待进程完全停止
    }

    // 构建命令
    std::string command = "tinycap /dev/stdout -d 0 -c 2 -r 48000 -b 16 | "
                          "LD_LIBRARY_PATH=. ./ffmpeg "
                          "-f s16le -ar 48000 -ac 2 -i pipe:0 "
                          "-c:a aac -b:a 192k "
                          "-f rtsp " + rtspUrl;

    std::cout << "启动音频流命令: " << command << std::endl;

    // 创建子进程
    audioPid = fork();

    if (audioPid == -1) {
        std::cerr << "创建子进程失败" << std::endl;
        return false;
    }

    if (audioPid == 0) {
        // 子进程 - 执行命令
        execl("/bin/sh", "sh", "-c", command.c_str(), (char *)NULL);

        // 如果execl失败
        std::cerr << "执行命令失败" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // 父进程
        std::cout << "音频流启动成功，进程PID: " << audioPid
                  << " RTSP URL: " << rtspUrl << std::endl;

        // 检查子进程是否正常运行（可选）
        sleep(1);
        if (kill(audioPid, 0) == 0) {
            return true;
        } else {
            audioPid = -1;
            return false;
        }
    }
}

void stopAudioStream()
{
    std::cout << "停止音频流" << std::endl;

    // 直接杀死所有相关进程
    system("killall -9 tinycap ffmpeg 2>/dev/null");

    audioPid = -1;
}


// 打印所有动作
void printActions(const vector<Dev_Action>& actions) {
    for (size_t i = 0; i < actions.size(); ++i) {
        cout << "Action " << i + 1 << ":" << endl;
        cout << "  action: " << actions[i].action << endl;
        cout << "  sub_action: " << actions[i].sub_action << endl;
        cout << "  start_time: " << actions[i].start_time << endl;
        cout << "  end_time: " << actions[i].end_time << endl;
        cout << "  remark: " << actions[i].remark << endl;
        cout << "  isRunning: " << (actions[i].isRunning ? "true" : "false") << endl;
        cout << "-------------------------" << endl;
    }
}
// 字符串转换为大写（用于MD5比较忽略大小写）
std::string toUpperString(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}
#include <algorithm>
#include <cctype>
#include <sys/stat.h>
// 获取文件大小
long getFileSize(const std::string& filepath) {
    struct stat stat_buf;
    int rc = stat(filepath.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

// 检查并创建目录
bool ensureDirectoryExists(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }

    // 目录不存在，尝试创建
    std::string cmd = "mkdir -p \"" + path + "\"";
    return system(cmd.c_str()) == 0;
}

void clearActions(vector<Dev_Action>& actions) {
    actions.clear(); // 清空所有元素
}
std::string getTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* tm = std::localtime(&now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", tm);
    return std::string(buffer);
}

void ParseMqttMassage(string paylaod, vector<Dev_Action> &actions)
{
    string messagetype = check_message_type(paylaod);
    std::cout << " 指令" << messagetype<<std::endl;

    if(messagetype == "command")
    {
        Dev_Action action;
        action.action = extract_json_field(paylaod, "action");
        action.sub_action = extract_json_field(paylaod, "sub_action");
        action.start_time = extract_json_field(paylaod, "start_time");
        action.end_time = extract_json_field(paylaod, "end_time");
        action.remark = extract_json_field(paylaod, "remark");
        action.command_id =extract_json_field(paylaod, "command_id");
        action.isRunning=false;
        action.isCommand = true;
        action.Forcestop = false;
        action.compeleted  = false;
        std::cout << "活动待运行:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        std::cout << "指令码:"<< action.command_id<< std::endl;
        if(!action.action.empty())
        {
            std::cout << "活动待运行:"<<action.action << action.sub_action<< std::endl;
            std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
            std::cout << "指令码:"<< action.command_id<< std::endl;
        actions.push_back(action);
        }
        else
        {
            std::cout << "无效指令:"<<action.action << action.sub_action<< std::endl;
            std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
            std::cout << "指令码:"<< action.command_id<< std::endl;
        }
    }
    else if(messagetype == "process")
    {
        clearActions(actions);

        parseDataArray(paylaod, actions);
        printActions(actions); //
    }
    else if(messagetype == "stream")
    {
        std::cout << " 消息类型: " << messagetype << std::endl;

        // 提取url和switch字段
        // 注意JSON格式可能有空格或没有空格，所以查找多种可能
        size_t url_start = paylaod.find("\"url\"");
        if(url_start != std::string::npos) {
            url_start = paylaod.find('"', url_start + 6); // 找到冒号后的第一个引号
            if(url_start != std::string::npos) {
                url_start += 1; // 跳过引号
                size_t url_end = paylaod.find('"', url_start);
                std::string url = paylaod.substr(url_start, url_end - url_start);

                std::cout << " 提取到URL: " << url << std::endl;

                // 查找switch字段
                size_t switch_start = paylaod.find("\"switch\"");
                if(switch_start != std::string::npos) {
                    switch_start = paylaod.find('"', switch_start + 9); // 找到冒号后的第一个引号
                    if(switch_start != std::string::npos) {
                        switch_start += 1; // 跳过引号
                        size_t switch_end = paylaod.find('"', switch_start);
                        std::string switch_value = paylaod.substr(switch_start, switch_end - switch_start);

                        std::cout << " 提取到switch: " << switch_value << std::endl;

                        if(switch_value == "on") {
                            std::cout << " 打开推流" << std::endl;
                            startAudioStream(url);
                        } else if(switch_value == "off") {
                            std::cout << " 关闭推流" << std::endl;
                            stopAudioStream();
                        } else {
                            std::cout << " 未知的switch值: " << switch_value << std::endl;
                        }
                    }
                }
            }
        } else {
            std::cout << " 未找到url字段" << std::endl;
        }
    }
    else if(messagetype == "upgrade")
    {
        UpgradeInfo info = parseJsonToUpgradeInfo(paylaod);
        if (info.isValid()) {
            std::cout << "解析成功！" << std::endl;
            info.print();
            std::cout << "\n提取的关键信息:" << std::endl;
            std::cout << "下载URL: " << info.url << std::endl;
            std::cout << "文件MD5: " << info.md5 << std::endl;
        } else {
            std::cout << "解析失败或缺少必要信息！" << std::endl;
            return;
        }

        std::string filepath = "/data/machine_vision/upgrade.tar.gz";

        // 检查并创建升级目录
        std::string upgrade_dir = "/data/machine_vision/";
        std::string ensure_dir_cmd = "mkdir -p " + upgrade_dir;
        system(ensure_dir_cmd.c_str());

        if(-1 == Downloadfile(info.url, filepath))
        {
            std::cerr << "文件下载失败" << std::endl;
            return;
        }

        std::cout << "文件下载完成: " << filepath << std::endl;

        // 计算本地文件的MD5值
        std::string md5hash2;
        try {
            md5hash2 = MD5::calculateFileMD5(filepath);
            std::cout << "本地文件MD5计算完成: " << md5hash2 << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "MD5计算失败: " << e.what() << std::endl;
            std::remove(filepath.c_str());
            return;
        }

        // 忽略大小写比较MD5值
        std::string local_md5_upper = toUpperString(md5hash2);
        std::string remote_md5_upper = toUpperString(info.md5);

        if(local_md5_upper != remote_md5_upper)
        {
            std::cout << "MD5校验失败:" << std::endl;
            std::cout << "本地MD5：" << md5hash2 << std::endl;
            std::cout << "云端MD5：" << info.md5 << std::endl;
            std::cout << "比对不一致" << std::endl;

            // 删除下载的文件
            std::remove(filepath.c_str());
            return;
        }

        std::cout << "MD5校验通过（忽略大小写）" << std::endl;

        // 执行解压命令 - 添加--no-same-owner避免权限错误
        std::string extract_cmd = "tar -xzf \"" + filepath + "\" -C \"" + upgrade_dir + "\" --no-same-owner --overwrite 2>&1";
        std::cout << "执行解压命令: " << extract_cmd << std::endl;

        // 执行解压命令并捕获输出
        FILE* pipe = popen(extract_cmd.c_str(), "r");
        if (!pipe) {
            std::cerr << "执行解压命令失败" << std::endl;
            std::remove(filepath.c_str());
            return;
        }

        char buffer[128];
        std::string extract_output = "";
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL) {
                extract_output += buffer;
            }
        }

        int extract_result = pclose(pipe);

        if (extract_result != 0) {
            std::cerr << "解压失败，返回值: " << extract_result << std::endl;
            if (!extract_output.empty()) {
                std::cerr << "错误信息: " << extract_output << std::endl;
            }
            std::remove(filepath.c_str());
            return;
        }

        if (!extract_output.empty()) {
            std::cout << "解压输出: " << extract_output << std::endl;
        }

        std::cout << "解压成功！文件已覆盖到 " << upgrade_dir << std::endl;

        // 修复文件所有权为root（可选）
        std::string fix_ownership_cmd = "chown -R root:root \"" + upgrade_dir + "\" 2>/dev/null";
        std::cout << "修复文件所有权: " << fix_ownership_cmd << std::endl;

        int fix_result = system(fix_ownership_cmd.c_str());
        if (fix_result != 0) {
            std::cout << "文件所有权修复完成（非root用户可能无权限）" << std::endl;
        } else {
            std::cout << "文件所有权已设置为root" << std::endl;
        }

        // 确保脚本有执行权限
        std::string fix_permissions_cmd = "chmod +x \"" + upgrade_dir + "\"/*.sh 2>/dev/null";
        system(fix_permissions_cmd.c_str());

        // 创建标记文件路径
        std::string timestamp_file = upgrade_dir + "upgrade_timestamp.txt";

        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        // 打开文件并写入执行时间
        std::ofstream time_file(timestamp_file, std::ios::out | std::ios::trunc);
        if (!time_file.is_open()) {
            std::cerr << "无法创建时间标记文件: " << timestamp_file << std::endl;
        } else {
            // 格式化的时间字符串
            std::tm tm_info;
            localtime_r(&now_time, &tm_info);
            char time_str[100];
            std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);

            // 写入执行时间
            time_file << "升级完成时间: " << time_str << std::endl;
            time_file << "原始压缩包: " << filepath << std::endl;
            time_file << "解压目标目录: " << upgrade_dir << std::endl;
            time_file << "文件MD5: " << info.md5 << std::endl;
            time_file << "本地MD5: " << md5hash2 << std::endl;
            time_file << "MD5校验: 通过" << std::endl;
            time_file << "文件大小: " << getFileSize(filepath) << " 字节" << std::endl;
            time_file.close();

            std::cout << "已创建升级时间标记文件: " << timestamp_file << std::endl;
        }

        // 删除下载的压缩包以节省空间
        if (std::remove(filepath.c_str()) == 0) {
            std::cout << "已清理临时压缩包: " << filepath << std::endl;
        } else {
            std::cerr << "警告：无法删除临时压缩包" << std::endl;
        }

        // 清理升级目录（如果有的话）
        std::string temp_upgrade_dir = upgrade_dir + "upgradeDir/";
        std::string rmdir_cmd = "rm -rf \"" + temp_upgrade_dir + "\" 2>/dev/null";
        system(rmdir_cmd.c_str());

        std::cout << "升级完成，准备重启系统..." << std::endl;

        // 记录重启日志
        std::ofstream reboot_log(upgrade_dir + "reboot_log.txt", std::ios::app);
        if (reboot_log.is_open()) {
            std::tm tm_info;
            localtime_r(&now_time, &tm_info);
            char time_str[100];
            std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);
            reboot_log << "[" << time_str << "] 升级完成，触发系统重启" << std::endl;
            reboot_log.close();
        }

        // 确保所有输出都已刷新
        std::cout.flush();
        std::cerr.flush();

        // 等待3秒，让日志完全写入
       // std::this_thread::sleep_for(std::chrono::seconds(3));
        sleep(3);
        // 重启系统
        std::cout << "正在重启系统..." << std::endl;
        system("sync");  // 同步文件系统，确保数据写入磁盘
        system("reboot");
    }
    else if(messagetype == "reboot")
    {
        system("reboot");
        cout << "get rebooot  ...."<< messagetype << endl;
    }

    else
    {
        cout << "err json ...."<< messagetype << endl;
    }

}
Dev_Action* TraverActionsVector(vector<Dev_Action>& actions , Dev_Action *&currentAct)
{
    if (actions.empty())  // 先检查 actions 是否为空
    {
        return nullptr;
    }

    for (auto it = actions.begin(); it != actions.end();)
    {
        auto& action = *it;
    // 注意 current会遍历到他自己！！！
        try {
            // 如果活动已结束且未运行，删除活动
            if (compareTime(action.end_time) >= 0 && !action.isRunning)
            {
                std::cout << "活动已经过期:" << action.action << action.sub_action << std::endl;
                std::cout << "开始时间:" << action.start_time << " 停止时间:" << action.end_time << std::endl;

                it = actions.erase(it);  // 删除元素后，it 自动指向下一个元素
            }
            else if (compareTime(action.start_time) >= 0 && currentAct==nullptr )  // 如果活动开始时间到且没有正在进行的活动，则启动
            {
                std::cout << "准备启动活动:" << action.action << action.sub_action << std::endl;
                std::cout << "调试:" << compareTime(action.start_time) << std::endl;
                auto now = std::chrono::system_clock::now();

                // 转换为time_t类型
                std::time_t now_time = std::chrono::system_clock::to_time_t(now);

                // 转换为本地时间
                std::tm* local_time = std::localtime(&now_time);

                // 输出格式化时间
                std::cout << "当前时间: "
                          << std::put_time(local_time, "%Y-%m-%d %H:%M:%S")
                          << std::endl;
                action.print();
                currentAct =&action;
                return &action;  // 返回指向活动的指针
            }
            else if (compareTime(action.start_time) >= 0 && action.isRunning == false &&(action.sub_action =="验证码"   ||action.sub_action =="弹幕"   ||action.sub_action =="退出") )
            {   //注意 action.isRunning 是新消息   current 是在处理的消息
                std::cout << "更新活动:" << currentAct->action<<currentAct->sub_action<<"更新为"<< action.sub_action << std::endl;
                //currentAct =&action;
                currentAct->sub_action = action.sub_action;
                currentAct->remark = action.remark;
                currentAct->isRunning=false;
                action.compeleted =true;
                cout << "活动MARK:" << currentAct->remark;
                return &action;  // 返回指向活动的指针
            }
            else
            {
                ++it;  // 默认遍历，跳过没有删除或启动的活动
            }
        }
        catch (const std::exception& e) {
            std::cerr << "错误：处理活动时发生异常：" << e.what() << std::endl;
            ++it;  // 即使发生异常，也继续遍历
        }
    }
    return nullptr;  // 如果没有找到需要启动的活动，返回 nullptr
}




void pollAndRemoveCompletedActions(vector<Dev_Action>& actions)
{
       //  std::cout << "开始时间" << std::endl;

    // 使用迭代器来删除元素
    for (auto it = actions.begin(); it != actions.end();) {
        auto& action = *it;
       //     action.print();
        if (it->compeleted) {
            // 删除compeleted为true的元素
            it = actions.erase(it);
            cout << "Deleted completed action" << endl;
        } else {
            ++it;  // 继续下一个元素
        }
    }
}
std::string createApplicationStatusJson(Dev_Action &cur, const std::string& status) {
    // 获取当前时间戳
    std::time_t now = std::time(nullptr);
    std::tm* gm_time = std::gmtime(&now);

    // 生成ISO 8601格式的时间戳
    std::ostringstream timestamp_ss;
    timestamp_ss << std::put_time(gm_time, "%Y-%m-%dT%H:%M:%SZ");
    std::string timestamp = timestamp_ss.str();

    // 构建JSON字符串
    std::string json = "{";
    json += "\"timestamp\": \"" + timestamp + "\",";
    json += "\"serial_number\":\""+parseSerialNumber()+"\",";
    json += "\"verification_code\": \"GXFC\",";
    json += "\"messageType\": \"appliaction_status\",";
    json += "\"data\": {";
    json += "\"app_name\": \"" + cur.action + "\",";
    json += "\"command_id\": \"" + cur.command_id + "\",";
    json += "\"status\": \"" + status + "\"";
    json += "}";
    json += "}";

    return json;
}
// 处理转义字符的版本
std::string unescapeJsonString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
            case '"': result += '"'; break;
            case '\\': result += '\\'; break;
            case '/': result += '/'; break;
            case 'b': result += '\b'; break;
            case 'f': result += '\f'; break;
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            default: result += str[i + 1]; break;
            }
            ++i; // 跳过下一个字符
        } else {
            result += str[i];
        }
    }
    return result;
}
std::string extractValue(const std::string& jsonString, const std::string& key) {
    // 构建搜索模式：只找键名部分
    std::string searchKey = "\"" + key + "\":";
    std::size_t start = jsonString.find(searchKey);

    if (start == std::string::npos) {
        return "";
    }

    // 跳过键名和冒号
    start += searchKey.length();

    // 跳过空格、制表符、换行等空白字符
    while (start < jsonString.length() &&
           (jsonString[start] == ' ' ||
            jsonString[start] == '\t' ||
            jsonString[start] == '\n' ||
            jsonString[start] == '\r')) {
        start++;
    }

    // 检查值是否是字符串（应该以双引号开头）
    if (start >= jsonString.length() || jsonString[start] != '"') {
        return "";
    }

    // 跳过开头的引号
    start++;

    // 查找结束的引号
    std::size_t end = start;
    while (end < jsonString.length() && jsonString[end] != '"') {
        // 如果遇到转义字符，跳过下一个字符
        if (jsonString[end] == '\\' && end + 1 < jsonString.length()) {
            end += 2; // 跳过转义字符和转义的内容
        } else {
            end++;
        }
    }

    if (end >= jsonString.length()) {
        return "";
    }

    // 提取字符串值
    return jsonString.substr(start, end - start);
}

// 解析 JSON 字符串并返回 UpgradeInfo 结构体
UpgradeInfo parseJsonToUpgradeInfo(const std::string& jsonString) {
    UpgradeInfo info;

    // 先尝试从data对象中提取url和md5
    std::size_t dataStart = jsonString.find("\"data\":");
    if (dataStart != std::string::npos) {
        // 找到data对象的开始大括号
        dataStart = jsonString.find("{", dataStart);
        if (dataStart != std::string::npos) {
            // 找到data对象的结束大括号
            std::size_t braceCount = 1;
            std::size_t dataEnd = dataStart + 1;

            while (dataEnd < jsonString.length() && braceCount > 0) {
                if (jsonString[dataEnd] == '{') braceCount++;
                else if (jsonString[dataEnd] == '}') braceCount--;
                dataEnd++;
            }

            if (braceCount == 0) {
                // 提取data对象的内容
                std::string dataContent = jsonString.substr(dataStart, dataEnd - dataStart);

                // 在data内容中查找url和md5
                info.url = extractValue(dataContent, "url");
                info.md5 = extractValue(dataContent, "md5");
            }
        }
    }

    // 如果从data对象中没有找到url或md5，则在整个JSON中查找（兼容性处理）
    if (info.url.empty()) {
        info.url = extractValue(jsonString, "url");
    }
    if (info.md5.empty()) {
        info.md5 = extractValue(jsonString, "md5");
    }

    // 从外层提取其他字段
    info.messageType = extractValue(jsonString, "messageType");
    info.password = extractValue(jsonString, "password");
    info.timestamp = extractValue(jsonString, "timestamp");
    info.username = extractValue(jsonString, "username");

    return info;
}

int getRandomInRange(int min, int max) {
    return rand() % (max - min + 1) + min;
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
    // 获取serial_number
    std::string serial_number = parseSerialNumber(); // 替换为实际接口调用

    std::string modified_url = download_url + "&serial_number=" + serial_number +"&token="+VERIFY_CODE;

    // 使用download方法
    if (client.download(modified_url, save_path, config)) {
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

    // 获取serial_number
    std::string serial_number = parseSerialNumber(); // 替换为实际接口调用

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
    std::cout << "Serial Number: " << serial_number << std::endl;

    // 修改：将serial_number作为额外参数上传
    // 方式1：如果是URL参数（推荐）
    std::string modified_url = upload_url + "&serial_number=" + serial_number +"&token="+VERIFY_CODE;
    auto response = client.upload(modified_url, file_path, "file", config);

    // 方式2：如果是表单字段（根据你的HttpClient接口选择）
    // auto response = client.upload(upload_url, file_path, "file", {{"serial_number", serial_number}}, config);

    if (response.ok()) {
        std::cout << "✓ File uploaded successfully!" << std::endl;
        std::cout << "  Status: " << response.status_code << std::endl;
        std::cout << "  Response: " << response.body << std::endl;

        // 你可以在这里处理返回的数据
    } else {
        std::cout << "✗ Upload failed!" << std::endl;
        std::cout << "  Error: " << response.error << std::endl;
        std::cout << "  Status: " << response.status_code << std::endl;
        std::cout << "  Response: " << response.body << std::endl;

        return -1;
    }

    // 清理
    HttpClient::cleanup();

    std::cout << "\n=== Upload Completed ===" << std::endl;
    return 0;
}
