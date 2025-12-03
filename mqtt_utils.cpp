#include "mqtt_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mqtt_massage_t mqtt_msg = {0};

int mqtt_connect(struct mosquitto **mosq, mqtt_config_t *config) {
    // 初始化 Mosquitto 库
    *mosq = mosquitto_new(NULL, true, NULL);
    if (*mosq == NULL) {
        fprintf(stderr, "Error: Unable to create mosquitto client\n");
        return MOSQ_ERR_NOMEM;
    }

    // 设置用户名和密码
    if (config->username && config->password) {
        mosquitto_username_pw_set(*mosq, config->username, config->password);
    }

    // 连接到 MQTT 服务器
    int ret = mosquitto_connect(*mosq, config->host, config->port, 60);
    if (ret != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Failed to connect to MQTT server: %s\n", mosquitto_strerror(ret));
        return ret;
    }

    return MOSQ_ERR_SUCCESS;
}

void mqtt_cleanup(struct mosquitto *mosq) {
    if (mosq != NULL) {
        mosquitto_destroy(mosq);
    }
    mosquitto_lib_cleanup();
}
// 订阅主题
int mqtt_subscribe(struct mosquitto *mosq, const char *topic, int qos) {
    int ret = mosquitto_subscribe(mosq, NULL, topic, qos);
    if (ret != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Failed to subscribe to topic: %s\n", mosquitto_strerror(ret));
        return ret;
    }
    return MOSQ_ERR_SUCCESS;
}



// 发布消息
int mqtt_publish(struct mosquitto *mosq, const char *topic, const char *message, int qos) {
    int ret = mosquitto_publish(mosq, NULL, topic, strlen(message), message, qos, false);
    if (ret != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Failed to publish message: %s\n", mosquitto_strerror(ret));
        return ret;
    }
    return MOSQ_ERR_SUCCESS;
}
// 消息接收回调
void mqtt_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    printf("Received message: %s on topic %s\n", (char *)message->payload, message->topic);
    strlcpy(mqtt_msg.message,(char *)message->payload,512);
    strlcpy(mqtt_msg.topic,(char *)message->topic,128);
    mqtt_msg.newmsg = true;
}

// 设置回调函数
void mqtt_set_message_callback(struct mosquitto *mosq) {
    mosquitto_message_callback_set(mosq, mqtt_message_callback);
}
