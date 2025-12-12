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
std::string GetdeviceInfoAndresJson()//TODO
{
    DeviceData devicedata;
    NetworkInfo netinfo =getNetworkInfo("wlan0");
    devicedata.ip = netinfo.ip;
    devicedata.mac = netinfo.mac;
    devicedata.totalTraffic = netinfo.formatTraffic();
    devicedata.error_flag= 0;
    devicedata.temperature= readTemperature();
    devicedata.serial_number = parseSerialNumber();
    devicedata.verification_code = "GXFC";
    devicedata.boot_time = getUptime();
    devicedata.usedProcess = "xxxxx";
    devicedata.ProcessID = "1";
    HeartbeatMessage heartbeat;
    heartbeat.timestamp =getUtcTimestamp();
    heartbeat.messageType="heart";
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
    string processId = extractString(json, "processId");

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
        actions.push_back(action); // 将填充好的 action 对象加入到容器中

        objStart = objEnd + 1; // 继续查找下一个对象
    }
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


void clearActions(vector<Dev_Action>& actions) {
    actions.clear(); // 清空所有元素
}


void ParseMqttMassage(string paylaod, vector<Dev_Action> &actions)
{
    string messagetype = check_message_type(paylaod);

    if(messagetype == "command")
    {
        Dev_Action action;
        action.action = extract_json_field(paylaod, "action");
        action.sub_action = extract_json_field(paylaod, "sub_action");
        action.start_time = extract_json_field(paylaod, "start_time");
        action.end_time = extract_json_field(paylaod, "end_time");
        action.remark = extract_json_field(paylaod, "remark");
        action.isRunning=false;
        action.isCommand = true;
        action.Forcestop = false;
        action.compeleted  = false;
        std::cout << "活动待运行:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;

        actions.push_back(action);
    }
    else if(messagetype == "process")
    {
        clearActions(actions);
        parseDataArray(paylaod, actions);
        printActions(actions); //
    }
    else
    {
        cout << "err json ...."<< messagetype << endl;
    }

}
Dev_Action* TraverActionsVector(vector<Dev_Action>& actions)
{
    if (actions.empty())  // 先检查 actions 是否为空
    {
        std::cerr << "错误：活动列表为空!" << std::endl;
        return nullptr;
    }

    for (auto it = actions.begin(); it != actions.end();)
    {
        auto& action = *it;

        try {
            // 如果活动已结束且未运行，删除活动
            if (compareTime(action.end_time) >= 0 && !action.isRunning)
            {
                std::cout << "活动已经过期:" << action.action << action.sub_action << std::endl;
                std::cout << "开始时间:" << action.start_time << " 停止时间:" << action.end_time << std::endl;

                it = actions.erase(it);  // 删除元素后，it 自动指向下一个元素
            }
            else if (compareTime(action.start_time) >= 0 && !action.isRunning)  // 如果活动开始时间到且没有正在进行的活动，则启动
            {
                std::cout << "准备启动活动:" << action.action << action.sub_action << std::endl;
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


void SchedulingProcess(struct Dev_Action *currentAct)
{
   //  currentAct->print();
    if(currentAct->isRunning==false)
    {
        std::cout << "开始活动:"<<currentAct->action << currentAct->sub_action<< std::endl;
        std::cout << "开始时间:"<<currentAct->start_time << "停止时间:"<<currentAct->end_time << std::endl;

        currentAct->print();
        currentAct->isRunning =true;
    }

    if (((compareTime(currentAct->end_time) >= 0 && currentAct->isRunning) && !currentAct->compeleted) || \
        (currentAct->Forcestop && !currentAct->compeleted))
    {

        std::cout << "完成活动:"<<currentAct->action << currentAct->sub_action<< std::endl;
        std::cout << "开始时间:"<<currentAct->start_time << "停止时间:"<<currentAct->end_time << std::endl;

        currentAct->compeleted=true;
        currentAct =nullptr;
        // currentAct.isRunning =false;
    }
 //   cout << "接口结束"<<endl;

}
/*
 *
 *

            // else if(action.isCommand && currentAct.action!= STANDBY_ACT &&  action.Forcestop ==false)//todo  以后可能有BUG
            // {

            //     cout <<"新命令开始：" << action.action << action.sub_action;
            //     cout <<"先退出" << action.action << action.sub_action;
            //     action.Forcestop =true;
            // }
        int diff = compareTime(action.end_time);
        if(diff >= 0&& action.isRunning)//大于结束时间且已经开始了停止活动
        {
            std::cout << "停止活动:"<<action.action << action.sub_action<< std::endl;
            std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
            action.Forcestop =true;

            continue;
        }


        if(ClearFinishedCommand(action, app_tiktok))
        {
            if(action.sub_action!="弹幕")
                STATUS= IDLE;
            // 删除当前元素
            it = actions.erase(it);
            devicedata.current_action.name=   "待命";
            devicedata.current_action.start_time=  "00:00:00";
            devicedata.current_action.end_time=   "00:00:00";
            continue;
        }
        //else
 *                //Dev_Action tmpaction = action;
                action.isRunning =true;
                if (action.action == "抖音")
                {
                    disposeMessageTickTOk(app_tiktok, action);
                    STATUS =APP_TIKTOK;
                }
                else if (action.action == "红牛")
                {
                    STATUS =APP_WECHAT;
                }
            Dev_Action tmpaction = action;
            tmpaction.sub_action="退出";
            disposeMessageTickTOk(app_tiktok, tmpaction);
            // 删除当前元素
            it = actions.erase(it);
            STATUS= IDLE;


*/
