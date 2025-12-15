#include "mqtt_thread.h"
#include <iostream>
#include <cstring>
#include "json_utils.h"


std::string t_topic;
std::string t_message;
bool t_newMsg;

void onMessageReceived(const std::string &topic, const std::string &message) {
   // std::cout << "Message received on topic [" << topic << "]: " << message << std::endl;
    t_topic = message;
    t_message =topic;
    t_newMsg =true;
}

void onReconnect() {
    std::cout << "Reconnected to MQTT broker." << std::endl;
}



MqttThread::MqttThread(const std::string &host, int port, const std::string &username, const std::string &password)
    : host_(host), port_(port), username_(username), password_(password), mosq_(nullptr), running_(false), messageCallback_(nullptr), reconnectCallback_(nullptr) {
    mosquitto_lib_init();

    setMessageCallback(onMessageReceived);
    setReconnectCallback(onReconnect);

    generateTopic();
    cout <<"publish_topic_ :"<< publish_topic_<<"\nsubscribe_topic_ :" <<subscribe_topic_ <<endl;
}

MqttThread::~MqttThread() {
    stop();
    if (mosq_ != nullptr) {
        mosquitto_destroy(mosq_);
    }
    mosquitto_lib_cleanup();
}

void MqttThread::start() {
    running_ = true;
    mqtt_thread_ = std::thread(&MqttThread::run, this);
}

void MqttThread::stop() {
    running_ = false;
    if (mqtt_thread_.joinable()) {
        mqtt_thread_.join();
    }
}

bool MqttThread::publish(const std::string &topic, const std::string &message, int qos) {
    if (mosq_ == nullptr) {
        std::cerr << "MQTT client is not initialized!" << std::endl;
        return false;
    }

    int ret = mosquitto_publish(mosq_, nullptr, topic.c_str(), message.size(), message.c_str(), qos, false);
    if (ret != MOSQ_ERR_SUCCESS) {
        std::cerr << "Failed to publish message: " << mosquitto_strerror(ret) << std::endl;
        return false;
    }
    return true;
}

bool MqttThread::subscribe(const std::string &topic, int qos) {
    if (mosq_ == nullptr) {
        std::cerr << "MQTT client is not initialized!" << std::endl;
        return false;
    }

    int ret = mosquitto_subscribe(mosq_, nullptr, topic.c_str(), qos);
    if (ret != MOSQ_ERR_SUCCESS) {
        std::cerr << "Failed to subscribe to topic: " << mosquitto_strerror(ret) << std::endl;
        return false;
    }
    return true;
}

void MqttThread::setMessageCallback(void (*callback)(const std::string &topic, const std::string &message)) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    messageCallback_ = callback;
}

void MqttThread::setReconnectCallback(void (*callback)(void)) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    reconnectCallback_ = callback;
}

bool MqttThread::isMessageComing( std::string &topic, std::string &message)
{
    if(t_newMsg)
    {
        topic =t_topic;
        message = t_message;
        t_newMsg = false;
        return true;
    }

    return false;;
}

bool MqttThread::pubMessage(string msg)
{
    return publish(publish_topic_, msg, 0);

}

void MqttThread::run() {
    connectToBroker();


    while (running_) {
        int ret = mosquitto_loop(mosq_, 1000, 1);
        if (ret != MOSQ_ERR_SUCCESS) {
            std::cerr << "Mosquitto loop error: " << mosquitto_strerror(ret) << std::endl;
            reconnect();
        }

    }
}

void MqttThread::connectToBroker() {
    mosq_ = mosquitto_new(nullptr, true, this);
    if (mosq_ == nullptr) {
        std::cerr << "Unable to create Mosquitto client." << std::endl;
        return;
    }

    if (!username_.empty() && !password_.empty()) {
        mosquitto_username_pw_set(mosq_, username_.c_str(), password_.c_str());
    }

    int ret = mosquitto_connect(mosq_, host_.c_str(), port_, 60);
    if (ret != MOSQ_ERR_SUCCESS) {
        std::cerr << "Failed to connect to MQTT broker: " << mosquitto_strerror(ret) << std::endl;
        return;
    }

    mosquitto_message_callback_set(mosq_, messageCallback);
    mosquitto_connect_callback_set(mosq_, stateCallback);

    std::cout << "Connected to MQTT broker: " << host_ << std::endl;
    subscribe(subscribe_topic_);

}

void MqttThread::reconnect() {
    std::cout << "Reconnecting to MQTT broker..." << std::endl;

    int ret = mosquitto_reconnect(mosq_);
    if (ret != MOSQ_ERR_SUCCESS) {
        std::cerr << "Reconnection failed: " << mosquitto_strerror(ret) << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    } else {
        std::cout << "Reconnected to MQTT broker." << std::endl;
        // Re-subscribe topics after reconnecting
        subscribe(subscribe_topic_);
    }
}

void MqttThread::generateTopic()
{
    publish_topic_="Device/Report/"+parseSerialNumber();
    subscribe_topic_="Device/Dispatch/"+parseSerialNumber();
}

void MqttThread::debugfunc()
{
    ;
}

void MqttThread::messageCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    MqttThread *self = static_cast<MqttThread *>(userdata);
    if (self && self->messageCallback_) {
        std::lock_guard<std::mutex> lock(self->callback_mutex_);
        self->messageCallback_(message->topic, std::string((char *)message->payload, message->payloadlen));
    }
}

void MqttThread::stateCallback(struct mosquitto *mosq, void *userdata, int status) {
    MqttThread *self = static_cast<MqttThread *>(userdata);
    if (status == MOSQ_ERR_SUCCESS) {
        std::cout << "Connected successfully!" << std::endl;

    } else {
        std::cerr << "Connection failed with status: " << status << std::endl;
        if (self && self->reconnectCallback_) {
            std::lock_guard<std::mutex> lock(self->callback_mutex_);
            self->reconnectCallback_();
        }
    }
}
