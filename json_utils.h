#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "pulic_def.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cstdio>   // for popen
#include <cstdlib>  // for exit
#include <array>

using namespace std;
// 定义一个结构体来存储网卡信息
struct NetworkInfo {
     string ip;
     string mac;
    unsigned long long totalTraffic; // 下载+上传的流量总和（字节为单位）

    // 添加函数来格式化流量
     string formatTraffic() const {
        double traffic = static_cast<double>(totalTraffic);
         string unit = "B"; // 字节
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "KB";
        }
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "MB";
        }
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "GB";
        }
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "TB";
        }
        if (traffic >= 1024) {
            traffic /= 1024;
            unit = "PB";
        }
        return  to_string(traffic) + " " + unit;
    }
};

// 定义结构体
struct Action {
    string name;         // 动作名称
    string start_time;   // 开始时间
    string end_time;     // 结束时间
};
// 定义结构体
struct Dev_Action {
    string action;         // 动作名称
    string sub_action;         // 动作名称
    string start_time;   // 开始时间
    string end_time;     // 结束时间
    string remark;     // 结束时间
};
struct DeviceData {
   //  string status;               // 设备状态（在线/离线）
    Action current_action;            // 当前动作
    Action next_action;               // 下一步动作
    string ip;                   // 设备当前IP地址
    string  temperature;                  // 当前温度
    string boot_time;            // 设备开机时间
    bool error_flag;                  // 异常标志
     string warning_message;      // 警告信息
     string totalTraffic;      // 警告信息
     string mac;      // 警告信息
     string serial_number;        // 用户编号（设备序列号）
     string verification_code;    // 验证码
     string usedProcess;
     string ProcessID;

   //  string runtime;
};

struct HeartbeatMessage {
     string timestamp;            // 时间戳
     string messageType;          // 消息类型
    DeviceData data;                  // 设备数据
};


// 生成心跳包的 JSON 字符串
 string generateHeartbeatJson(const HeartbeatMessage& heartbeat);
 string readTemperature();
 NetworkInfo parseIfconfigOutput(const  string& interfaceName);
 string parseSerialNumber() ;
 string getUtcTimestamp();
 string getCurrentTime();
 string execCommand(const std::string& cmd);
 NetworkInfo getNetworkInfo(const std::string& interface);
 std::string getUptime() ;
 string check_message_type(const string& json_data);
 string extract_json_field(const string& json_data, const string& field_name);





#endif // OPENCV_UTILS_H
