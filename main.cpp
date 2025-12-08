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

int main() {
#if 1
    class APP_TIKTOK app;
    app.start();
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
                  if( action.action == "抖音")
                  {
                     // cout <<"获取命令1：" << action.action << action.sub_action <<endl;

                    if(action.sub_action=="私信")
                      {
                        app.remark =action.remark;
                        app.COMMAND = APP_TIKTOK::ACTING_COMMAND::SEND_MESSAGE;
                        cout <<"获取命令2：" << action.action << action.sub_action <<endl;
                      }
                    else if(action.sub_action=="直播")
                    {
                        app.remark =action.remark;
                        app.COMMAND = APP_TIKTOK::ACTING_COMMAND::LVIVINGROOM_ONSITE;
                        cout <<"获取命令2：" << action.action << action.sub_action <<endl;
                    }
                    else if(action.sub_action=="评论")
                     {
                          app.remark =action.remark;
                          app.COMMAND = APP_TIKTOK::ACTING_COMMAND::CONTENT_OPTRATION;
                          cout <<"获取命令2：" << action.action << action.sub_action <<endl;
                    }
                    else if(action.sub_action=="弹幕" )
                    {
                        app.remark =action.remark;
                        app.COMMAND = APP_TIKTOK::ACTING_COMMAND::LVIVINGROOM_BULLET;
                        cout <<"获取命令2：" << action.action << action.sub_action <<endl;
                    }
                    else if(action.sub_action=="退出" )
                    {
                        app.remark =action.remark;
                        app.COMMAND = APP_TIKTOK::ACTING_COMMAND::QUIT;
                        cout <<"获取命令2：" << action.action << action.sub_action <<endl;
                    }
                  }
              }
              else if(messagetype == "process")
              {

              }
              else
              {
                  cout << "err json ...."<< messagetype << endl;
              }
            mqtt_msg.newmsg=false;
            strcpy(mqtt_msg.message,"");
        }
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
