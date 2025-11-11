#include <stdio.h>

#include "pulic_def.h"
#include <unistd.h>
#include "screen_tap.h"
#include "app_tiktok.h"
#include "machine_vision.h"


int main()
{
    // APP_TIKTOK app;

    // app.start();  // 启动线程

    // std::this_thread::sleep_for(std::chrono::seconds(5));  // 主线程等待5秒

    // app.stop();  // 停止线程
    DEBUG_TAP_OPERATIONS();
    INPUT_HOME();
    sleep(3);
    //AppType app =1;
    turnon_application(APP_TIKTOK);
    return 0;
}



