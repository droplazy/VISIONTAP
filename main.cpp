#include <unistd.h>
#include "app_tiktok.h"
#include "json_utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "mqtt_thread.h"


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
        else if (action.sub_action == "关注")
        {
            app.remark = action.remark;
            app.COMMAND = APP_TIKTOK::ACTING_COMMAND::FOLLOW_SOMEONE;
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


void printSubActions(const vector<Dev_Action>& actions_vector) {
    for (const auto& action : actions_vector) {
        cout << action.sub_action << endl;  // 打印每个 Dev_Action 的 sub_action
        cout << action.start_time << endl;  // 打印每个 Dev_Action 的 sub_action
        cout << action.end_time << endl;  // 打印每个 Dev_Action 的 sub_action

    }
}

int main()
{
    DeviceData devicedata ;
    vector<Dev_Action> actions_vector;
    actions_vector.reserve(24);
    string processId;
    MqttThread mqttClient("192.168.10.103", 1883, "username", "password");
    mqttClient.start();
    string mqtt_topic;
    string mqtt_message;
    struct Dev_Action *currentAct = nullptr;
    bool aaa =false;
    // 主循环：任务调度
    while (1)
    {
        if(mqttClient.isMessageComing(mqtt_message,mqtt_topic))
        {
         //    std::cout << "Message received on topic [" << mqtt_topic << "]: " << mqtt_message << std::endl;
          ParseMqttMassage(mqtt_message,actions_vector);

        }

        if(currentAct == nullptr)
        {
            currentAct  =  TraverActionsVector(actions_vector);
        }
        else if(currentAct != nullptr)
        {
            SchedulingProcess(currentAct);
           cout << "当前活动:" << currentAct->sub_action <<endl;

         //  currentAct->print();
        }
        else
        {
            cout << "没有活动" <<endl;
        }

         printSubActions(actions_vector);
         pollAndRemoveCompletedActions(actions_vector);//清除已经结束或者无效的动作
        sleep(1);
    }

    mqttClient.stop();

    return 0;
}

#if 0
        TraverActionsVector(actions_vector,currentAct);

        if (currentAct != nullptr) {
          SchedulingProcess(currentAct);
            std::cout << "当前活动：" << currentAct.sub_action <<  std::endl;
          //  currentActprint();

        } else {
            std::cout << "当前没有活动" << std::endl;
        }
      //
        if(mqtt_msg.newmsg)
        {

        }
#endif
#if 0




#endif
#if 0

    class APP_TIKTOK app_tiktok;
    devicedata.current_action.name=   "待命";
    devicedata.current_action.start_time= "00:00:00";
    devicedata.current_action.end_time=   "00:00:00";

    app_tiktok.start();
    getdeviceInfo();
#endif
