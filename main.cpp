//#include "pulic_def.h"
#include <unistd.h>
#include "app_tiktok.h"
#include "machine_vision.h"
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

int main() {

#if 0
    struct mosquitto *mosq = NULL;
    mqtt_config_t config = {
        .host = "test.mosquitto.org", // 替换为你自己的 MQTT 服务器地址
        .port = 1883,
        .username = NULL,  // 如果有用户名，设置此字段
        .password = NULL,  // 如果有密码，设置此字段
        .qos = 1
    };
    // 初始化 Mosquitto 并连接
    int ret = mqtt_connect(&mosq, &config);
    if (ret != MOSQ_ERR_SUCCESS) {
        return ret;
    }

    // 设置消息回调
    mqtt_set_message_callback(mosq);

    // 订阅主题
    ret = mqtt_subscribe(mosq, "test/topic123", config.qos);
    if (ret != MOSQ_ERR_SUCCESS) {
        mqtt_cleanup(mosq);
        return ret;
    }

    // 发布一条消息
    ret = mqtt_publish(mosq, "test/topic123", "来自设备端", config.qos);
    if (ret != MOSQ_ERR_SUCCESS) {
        mqtt_cleanup(mosq);
        return ret;
    }

    // 主循环：轮询接收消息
    while (1) {
        // 调用 mosquitto_loop 进行消息处理（阻塞直到有消息或超时）
        ret = mosquitto_loop(mosq, 0, 1);  // -1: 永远阻塞，1: 轮询处理一次消息
        if (ret != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Error in mosquitto_loop: %s\n", mosquitto_strerror(ret));
            break;
        }
        cout <<mqtt_msg.message<< endl;

        if(strcmp(mqtt_msg.message,"来自PC端")==0)
        {
            cout <<"我是非阻塞....\n" << endl;


            sleep(5);
            exit(0);
        }


        // 你可以在这里添加一些自定义的逻辑，比如检查某个条件来退出循环
        // 例如：检测是否达到一定的时间限制，或者监听某个停止命令
        sleep(1); // 控制轮询频率（可根据需求调整）
    }

    // 停止循环并清理
    mosquitto_loop_stop(mosq, true);
    mqtt_cleanup(mosq);
#endif

    class APP_TIKTOK app;
    app.start();
    //copyToClipboard("我是张家豪");
    return 0;
}


