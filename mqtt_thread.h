#ifndef MQTT_THREAD_H
#define MQTT_THREAD_H

#include <thread>
#include <mosquitto.h>
#include <atomic>
#include <string>
#include <chrono>
#include <mutex>

class MqttThread {
public:
    MqttThread(const std::string &host, int port, const std::string &username = "", const std::string &password = "");
    ~MqttThread();

    void start();
    void stop();

    bool publish(const std::string &topic, const std::string &message, int qos = 0);
    bool subscribe(const std::string &topic, int qos = 0);

    void setMessageCallback(void (*callback)(const std::string &topic, const std::string &message));
    void setReconnectCallback(void (*callback)(void));
    bool isMessageComing(std::string &topic, std::string &message);
private:
    void run();
    void connectToBroker();
    void reconnect();
    void generateTopic();
    void debugfunc();
    static void messageCallback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
    static void stateCallback(struct mosquitto *mosq, void *userdata, int status);


    struct mosquitto *mosq_;
    std::string host_;
    int port_;
    std::string username_;
    std::string publish_topic_;
    std::string subscribe_topic_;

    std::string password_;
    std::atomic<bool> running_;
    std::thread mqtt_thread_;
    std::mutex callback_mutex_;


    void (*messageCallback_)(const std::string &topic, const std::string &message);
    void (*reconnectCallback_)();
    // static void onReconnect();
    // static void onMessageReceived(const std::string &topic, const std::string &message);
};

#endif // MQTT_THREAD_H
