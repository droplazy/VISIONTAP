#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "public_def.h"
#include <string>


using namespace std;
// 升级信息结构体
struct UpgradeInfo {
    std::string url;
    std::string md5;
    std::string messageType;
    std::string password;
    std::string timestamp;
    std::string username;

    // 打印信息
    void print() const {
        std::cout << "升级信息:" << std::endl;
        std::cout << "  URL: " << url << std::endl;
        std::cout << "  MD5: " << md5 << std::endl;
        std::cout << "  消息类型: " << messageType << std::endl;
        std::cout << "  密码: " << password << std::endl;
        std::cout << "  时间戳: " << timestamp << std::endl;
        std::cout << "  用户名: " << username << std::endl;
    }

    // 检查必要字段
    bool isValid() const {
        return !url.empty() && !md5.empty();
    }
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
 std::string GetdeviceInfoAndresJson(Dev_Action *cur);
 void printActions(const vector<Dev_Action>& actions);
 void parseDataArray(const string& json, vector<Dev_Action>& actions);
 string extractString(const string& json, const string& key);
 void clearActions(vector<Dev_Action>& actions);
 void ParseMqttMassage(string paylaod, vector<Dev_Action> &actions);
 Dev_Action* TraverActionsVector(vector<Dev_Action>& actions, Dev_Action* &currentAct);
 void pollAndRemoveCompletedActions(vector<Dev_Action>& actions);
 int compareTime(const std::string& target);
 // void SchedulingProcess(struct Dev_Action *currentAct ,ThreadBase *p_thread);
 int getRandomInRange(int min, int max) ;
 std::string getTimestamp();
 UpgradeInfo parseJsonToUpgradeInfo(const std::string& json);
std::string createApplicationStatusJson(Dev_Action &cur, const std::string& status);
 std::string getdeviceIp() ;
 int Uploadfile(const std::string& upload_url, const std::string& file_path);
 int Downloadfile(std::string download_url,std::string save_path);

#endif // OPENCV_UTILS_H
