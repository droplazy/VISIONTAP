#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "public_def.h"
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
#endif // OPENCV_UTILS_H
