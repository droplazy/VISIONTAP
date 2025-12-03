#include "json_utils.h"
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
         << "    \"next_action\": {\n"
         << "      \"name\": \"" << heartbeat.data.next_action.name << "\",\n"
         << "      \"start_time\": \"" << heartbeat.data.next_action.start_time << "\",\n"
         << "      \"end_time\": \"" << heartbeat.data.next_action.end_time << "\"\n"
         << "    },\n"
         << "    \"ip\": \"" << heartbeat.data.ip << "\",\n"
         << "    \"temperature\": " << heartbeat.data.temperature << ",\n"
         << "    \"boot_time\": \"" << heartbeat.data.boot_time << "\",\n"
         << "    \"error_flag\": " << (heartbeat.data.error_flag ? "true" : "false") << ",\n"
         << "    \"warning_message\": \"" << heartbeat.data.warning_message << "\",\n"
         << "    \"mac\": \"" << heartbeat.data.mac << "\",\n"
       //  << "    \"runtime\": \"" << heartbeat.data.runtime << "\"\n"    devicedata.ProcessID = "P001";
         << "    \"traffic_statistics\": \"" << heartbeat.data.totalTraffic << "\",\n"
         << "    \"usedProcess\": \"" << heartbeat.data.usedProcess << "\",\n"
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
        cout << "messageType not found!" << endl;
        return "";
    }
    cout << "Found \"messageType\": at position " << pos << endl;

    // 从字段位置开始，提取 "messageType" 的值
    pos = json_data.find(":", pos);
    if (pos == string::npos) {
        cout << "Colon (:) not found after \"messageType\"" << endl;
        return "";
    }
    cout << "Found colon (:) at position " << pos << endl;

    // 跳过冒号，找到第一个非空字符（去掉空格和引号）
    pos = json_data.find_first_not_of(" \t\n\r", pos + 1);
    if (pos == string::npos) {
        cout << "No non-whitespace character found after colon" << endl;
        return "";
    }
    cout << "Found non-whitespace character at position " << pos << endl;

    // 查找字段的结束位置，假设是双引号或其他标记
    size_t end_pos = json_data.find_first_of("\"", pos + 1);
    if (end_pos == string::npos) {
        cout << "End quote for \"messageType\" not found!" << endl;
        return "";
    }
    cout << "Found end quote at position " << end_pos << endl;

    // 提取字符串，返回 messageType 的值
    string message_type = json_data.substr(pos + 1, end_pos - pos - 1);
    cout << "Extracted messageType: " << message_type << endl;

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
