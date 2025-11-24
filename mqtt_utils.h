#ifndef MQTT_UTILS_H
#define MQTT_UTILS_H

#include "mosquitto.h"

// 定义连接配置的结构体
typedef struct {
    const char *host;     // MQTT 服务器地址
    int port;             // MQTT 服务器端口
    const char *username; // 用户名
    const char *password; // 密码
    int qos;              // QoS（质量服务等级）
} mqtt_config_t;

typedef struct {
     bool newmsg;
     char topic[128]; //
     char message[512]; //
} mqtt_massage_t;

extern mqtt_massage_t mqtt_msg;


// 初始化 Mosquitto 客户端并连接到 MQTT 服务器
int mqtt_connect(struct mosquitto **mosq, mqtt_config_t *config);

// 清理 Mosquitto 客户端
void mqtt_cleanup(struct mosquitto *mosq);

int mqtt_publish(struct mosquitto *mosq, const char *topic, const char *message, int qos) ;
void mqtt_set_message_callback(struct mosquitto *mosq) ;
void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
int mqtt_subscribe(struct mosquitto *mosq, const char *topic, int qos) ;




#endif // MQTT_UTILS_H
