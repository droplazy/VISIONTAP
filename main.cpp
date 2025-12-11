//#include "pulic_def.h"
#include <unistd.h>
#include "app_tiktok.h"
#include "json_utils.h"
#include "mqtt_utils.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "mosquitto.h"
#include "mqtt_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct mosquitto *mosq = NULL;
mqtt_config_t config = {
    .host = "test.mosquitto.org", // 替换为你自己的 MQTT 服务器地址
    .port = 1883,
    .username = NULL,  // 如果有用户名，设置此字段
    .password = NULL,  // 如果有密码，设置此字段
    .qos = 1
};
DeviceData devicedata ;
vector<Dev_Action> actions;
RUNNUNG_APP STATUS = IDLE;
string processId;

int isConnected =0;

// 初始化 Mosquitto 并连接


void getdeviceInfo()
{
    struct Action current = { "吃饭", "2025-10-25T12:00:12Z", "2025-10-25T13:13:22Z" };
    struct Action next = { "洗澡", "2025-10-25T14:00:12Z", "2025-10-25T14:13:22Z" };
    devicedata.current_action = current;
    devicedata.next_action = next;
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
    //  devicedata.runtime =
    return ;
}

int publishHeart()
{
    getdeviceInfo();
    HeartbeatMessage heartbeat;
    heartbeat.timestamp =getUtcTimestamp();
    heartbeat.messageType="heart";
    heartbeat.data =devicedata;
    string payload =  generateHeartbeatJson(heartbeat);
    // 发布一条消息
    string topic ="Device/Report/" +devicedata.serial_number;
    int ret = mqtt_publish(mosq, topic.c_str(),payload.c_str(), config.qos);
    if (ret != MOSQ_ERR_SUCCESS) {
        mqtt_cleanup(mosq);
        return ret;
    }
    // cout << "topic : \n " << topic << endl;
    // cout << "payload : \n " << payload << endl;


    return 0;
}
// 订阅主题parseSerialNumber();
int SubscribeServerMsg() {

    string topic ="Device/Dispatch/" +devicedata.serial_number;
    cout << "尝试订阅: " << topic<<endl;
    int ret =mqtt_subscribe(mosq,topic.c_str(), config.qos);
    if(ret == MOSQ_ERR_SUCCESS)
    {
        cout << "订阅成功: " <<ret   <<"主题"<< topic<<endl;

        return 0;

    }
    cout << "订阅失败: "<< ret<< topic<<endl;
    return -1;
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
void parseDataArray(const string& json, vector<Dev_Action>& actions, string& processId) {
    // 提取 processId
    processId = extractString(json, "processId");

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

int disposeMessageTickTOk(class APP_TIKTOK &app, Dev_Action &action)
{
    int retCode = 0; // 0=未处理，1=成功，负值=错误
    try {
            if (action.sub_action == "私信")
            {
                app.remark = action.remark;
                app.COMMAND = APP_TIKTOK::ACTING_COMMAND::SEND_MESSAGE;
                cout << "获取命令2：" << action.action << action.sub_action << endl;
                retCode = 1;
            }
            else if (action.sub_action == "直播")
            {
                app.remark = action.remark;
                app.COMMAND = APP_TIKTOK::ACTING_COMMAND::LVIVINGROOM_ONSITE;
                cout << "获取命令2：" << action.action << action.sub_action << endl;
                retCode = 1;
            }
            else if (action.sub_action == "评论")
            {
                app.remark = action.remark;
                app.COMMAND = APP_TIKTOK::ACTING_COMMAND::CONTENT_OPTRATION;
                cout << "获取命令2：" << action.action << action.sub_action << endl;
                retCode = 1;
            }
            else if (action.sub_action == "弹幕")
            {
                app.remark = action.remark;
                app.COMMAND = APP_TIKTOK::ACTING_COMMAND::LVIVINGROOM_BULLET;
                cout << "获取命令2：" << action.action << action.sub_action << endl;
                retCode = 1;
            }
            else if (action.sub_action == "退出")
            {
                app.remark = action.remark;
                app.COMMAND = APP_TIKTOK::ACTING_COMMAND::QUIT;
                cout << "获取命令2：" << action.action << action.sub_action << endl;
                retCode = 1;
            }
            else if (action.sub_action == "互粉")
            {
                app.remark = action.remark;
                app.COMMAND = APP_TIKTOK::ACTING_COMMAND::FOLLOW_MODE;
                cout << "获取命令2：" << action.action << action.sub_action << endl;
                retCode = 1;
            }
            else if (action.sub_action == "刷视频")
            {
                app.remark = action.remark;
                app.COMMAND = APP_TIKTOK::ACTING_COMMAND::SCROLLING_MODE;
                cout << "获取命令2：" << action.action << action.sub_action << endl;
                retCode = 1;
            }
            else
            {
                std::cerr << "未知的sub_action: " << action.sub_action << std::endl;
                retCode = -2;
            }
    }
    catch (const std::exception& e) {
        std::cerr << "disposeMessageTickTOk异常: " << e.what() << std::endl;
        retCode = -999;
    }

    return retCode;
}
bool ClearFinishedCommand(Dev_Action & action, class APP_TIKTOK &app_tiktok)
{
    if(action.isRunning&& action.action=="抖音"&&app_tiktok.COMMAND ==APP_TIKTOK::ACTING_COMMAND::NONE)
    {//todo
        std::cout << "活动已经提前结束:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        return true;
    }
    if(action.isRunning&& action.action=="抖音"&&action.sub_action=="私信"&&app_tiktok.COMMAND !=APP_TIKTOK::ACTING_COMMAND::SEND_MESSAGE)
    {//todo
        std::cout << "活动已经提前结束:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        return true;
    }
    if(action.isRunning&& action.action=="抖音"&&action.sub_action=="弹幕"&&app_tiktok.COMMAND !=APP_TIKTOK::ACTING_COMMAND::LVIVINGROOM_BULLET)
    {//todo
        std::cout << "活动已经提前结束:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        return true;
    }
    if(action.isRunning&& action.action=="抖音"&&action.sub_action=="评论"&&app_tiktok.COMMAND !=APP_TIKTOK::ACTING_COMMAND::CONTENT_OPTRATION)
    {//todo
        std::cout << "活动已经提前结束:"<<action.action << action.sub_action<< std::endl;
        std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;
        return true;
    }
    return false;;
}
void clearActions(vector<Dev_Action>& actions) {
    actions.clear(); // 清空所有元素
}
int main() {
#if 1

    class APP_TIKTOK app_tiktok;

    app_tiktok.start();
    getdeviceInfo();
    int ret = mqtt_connect(&mosq, &config);
    if (ret != MOSQ_ERR_SUCCESS) {
        return ret;
    }

    // 设置消息回调
    mqtt_set_message_callback(mosq);

    SubscribeServerMsg();
    // // 发布一条消息
    // ret = mqtt_publish(mosq, "test/topic123", "来自设备端", config.qos);
    // if (ret != MOSQ_ERR_SUCCESS) {
    //     mqtt_cleanup(mosq);
    //     return ret;
    // }
    /***********************************************************************/
    auto last_time = std::chrono::steady_clock::now();

    // 主循环：轮询接收消息
    while (1) {
        // 调用 mosquitto_loop 进行消息处理（阻塞直到有消息或超时）
        ret = mosquitto_loop(mosq, 0, 1);  // -1: 永远阻塞，1: 轮询处理一次消息
        if (ret != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "Error in mosquitto_loop: %s\n", mosquitto_strerror(ret));
            // 尝试重新连接
            while (1)
            {
                cout << "网络错误尝试重新连接....." << endl;
                ret = mqtt_connect(&mosq, &config);
                if (ret != MOSQ_ERR_SUCCESS) {
                    continue;
                }
                else
                {
                    cout << "连接成功....." <<endl;
                    SubscribeServerMsg();
                    break;
                }
            }
        }

        if(mqtt_msg.newmsg)
        {
            string messagetype = check_message_type(mqtt_msg.message);

            if(messagetype == "command")
            {
                Dev_Action action;
                action.action = extract_json_field(mqtt_msg.message, "action");
                action.sub_action = extract_json_field(mqtt_msg.message, "sub_action");
                action.start_time = extract_json_field(mqtt_msg.message, "start_time");
                action.end_time = extract_json_field(mqtt_msg.message, "end_time");
                action.remark = extract_json_field(mqtt_msg.message, "remark");
                action.isRunning=false;
                action.isCommand = true;
                actions.push_back(action);
            }
            else if(messagetype == "process")
            {
                clearActions(actions);

                parseDataArray(mqtt_msg.message, actions, processId);
                printActions(actions); //
            }
            else
            {
                cout << "err json ...."<< messagetype << endl;
            }
            mqtt_msg.newmsg=false;
            strcpy(mqtt_msg.message,"");
        }
#if 1

        for (auto it = actions.begin(); it != actions.end(); )
        {
            auto& action = *it;
            int diff = compareTime(action.end_time);
            if(diff >= 0&& action.isRunning)//大于结束时间且已经开始了停止活动
            {
                std::cout << "停止活动:"<<action.action << action.sub_action<< std::endl;
                std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;

                Dev_Action tmpaction = action;
                tmpaction.sub_action="退出";
                disposeMessageTickTOk(app_tiktok, tmpaction);
                // 删除当前元素
                it = actions.erase(it);
                STATUS= IDLE;
                continue;
            }
            else if (diff >= 0&& !action.isRunning)
            {
                std::cout << "无效活动:"<<action.action << action.sub_action<< std::endl;
                std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;

                // 删除当前元素
                it = actions.erase(it);
                continue;
            }
            diff = compareTime(action.start_time);
            if(diff >= 0 && !action.isRunning) //大于开始时间但是还没有开始直接启动
            {
                if(STATUS== IDLE||(action.isCommand && STATUS== APP_TIKTOK&& action.sub_action=="弹幕") )
                {
                    std::cout << "开始活动:"<<action.action << action.sub_action<< std::endl;
                    std::cout << "开始时间:"<<action.start_time << "停止时间:"<<action.end_time << std::endl;

                    //Dev_Action tmpaction = action;
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
                }
                else if(action.isCommand && STATUS!= IDLE)//todo  以后可能有BUG
                {

                    // cout <<"新命令开始：" << action.action << action.sub_action;
                    // cout <<"先退出" << action.action << action.sub_action;

                    if(STATUS == APP_TIKTOK)
                    {
                        app_tiktok.COMMAND =APP_TIKTOK::QUIT;
                    }

                }
            }

            if(ClearFinishedCommand(action, app_tiktok))
            {
                if(action.sub_action!="弹幕")
                STATUS= IDLE;
                // 删除当前元素
                it = actions.erase(it);
                continue;
            }
            //else
                ++it;
        }
#endif
        // 获取当前时间
        auto current_time = std::chrono::steady_clock::now();

        // 判断是否已经过去了3秒
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time).count() >= 5) {
            // 每3秒调用一次publishHeart()
            publishHeart();
            last_time = current_time;  // 更新上次调用时间
        }


    }

    // 停止循环并清理
    mosquitto_loop_stop(mosq, true);
    mqtt_cleanup(mosq);
#endif


    //copyToClipboard("我是张家豪");
    return 0;
}
