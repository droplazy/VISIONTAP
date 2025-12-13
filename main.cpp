#include <unistd.h>
#include "json_utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "mqtt_thread.h"
#include "ThreadBase.h"
#include "Thread_Tikok.h"

void SchedulingProcess(struct Dev_Action *currentAct ,ThreadBase *&p_thread)
{
    //  currentAct->print();
    if(currentAct->isRunning==false)
    {
        std::cout << "开始活动:"<<currentAct->action << currentAct->sub_action<< std::endl;
        std::cout << "开始时间:"<<currentAct->start_time << "停止时间:"<<currentAct->end_time << std::endl;

        currentAct->print();
        currentAct->isRunning =true;

        if(currentAct->action == "抖音")
        {
            p_thread = new Thread_Tikok("tiktok_thread",*currentAct);
            p_thread->start();
        }


    }

    if (((compareTime(currentAct->end_time) >= 0 && currentAct->isRunning) && !currentAct->compeleted) || \
        (currentAct->Forcestop && !currentAct->compeleted))
    {

        std::cout << "完成活动:"<<currentAct->action << currentAct->sub_action<< std::endl;
        std::cout << "开始时间:"<<currentAct->start_time << "停止时间:"<<currentAct->end_time << std::endl;

        currentAct->compeleted=true;
        currentAct =nullptr;
        // currentAct.isRunning =false;
    }
    //   cout << "接口结束"<<endl;

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
    ThreadBase *p_applation =nullptr;
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
            SchedulingProcess(currentAct,p_applation);
        //安全退出线程 方法  先等app状态标志为退出成功后在这里 调用safestop 重要
        //cout << "当前活动:" << currentAct->sub_action <<endl;
        if(p_applation!= nullptr)
        {
            if(p_applation->applacationstate ==ThreadBase::AppState::EXITING)
            {
                cout <<"线程已经退出..." <<endl;
                p_applation->safeStop();
                p_applation =nullptr;
            }
            else
            {
                switch(p_applation->applacationstate) {
                case ThreadBase::AppState::STARTING:
                    cout << "[DEBUG] App状态: STARTING (启动中)" << endl;
                    break;
                case ThreadBase::AppState::IDLE:
                    cout << "[DEBUG] App状态: IDLE (空闲中)" << endl;
                    break;
                case ThreadBase::AppState::BUSY:
                    cout << "[DEBUG] App状态: BUSY (忙碌中)" << endl;
                    break;
                case ThreadBase::AppState::EXITING:
                    cout << "[DEBUG] App状态: EXITING (退出中)" << endl;
                    break;
                case ThreadBase::AppState::ERROR:
                    cout << "[ERROR] App状态: ERROR (错误状态)" << endl;
                    break;
                default:
                    cout << "[WARN] App状态: UNKNOWN (未知状态)" << endl;
                    break;
                }
            }
        }
        else
        {
            cout << "p_applation 指针为空" <<endl;
        }
         //  currentAct->print();
        }
        else
        {
            cout << "没有活动" <<endl;
        }

       //  printSubActions(actions_vector); 打印出所有任务队列
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
