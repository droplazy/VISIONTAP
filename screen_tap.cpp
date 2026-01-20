#include "screen_tap.h"


int INPUT_TAP(ad_point pos) {
    // 构建 input tap 命令字符串
    char command[64];
    snprintf(command, sizeof(command), "input tap %d %d", pos.x, pos.y);
    cout << "tap :" << pos.x << "," << pos.y << "\n"<<endl;
    // 调用 system 执行命令
    return system(command);
}

int INPUT_TAP_DELAY(ad_point pos,int delay ) {
    // 构建 input tap 命令字符串
    char command[64];
    snprintf(command, sizeof(command), "input swipe  %d %d %d %d %d ", pos.x, pos.y,pos.x, pos.y,delay);

    // 调用 system 执行命令
    return system(command);
}

int INPUT_SWIPE(ad_point start, ad_point end, int duration) {
    // 构建 input swipe 命令字符串
    char command[128];
    snprintf(command, sizeof(command), "input swipe %d %d %d %d %d", 
             start.x, start.y, end.x, end.y, duration);
    cout << command <<endl;

    // 调用 system 执行命令
    return system(command);
}

int INPUT_HOME() {
    return system("input keyevent 3");  // 3 是 HOME 键的 keycode
}

// BACK键事件
int INPUT_BACK() {
    return system("input keyevent 4");  // 4 是 BACK 键的 keycode
}
// BACK键事件
int INPUT_TASKAPP() {
    return system("input keyevent KEYCODE_APP_SWITCH");  // 4 是 BACK 键的 keycode
}
#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 600

// 打开文件管理器并从中间向上滑动200像素
int OPEN_FILE_MANAGER() {
    ad_point start = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};  // 屏幕中心
    ad_point end = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 200};  // 向上滑动200像素
    int duration = 300;  // 可调整滑动时长
    // 假设这里有打开文件管理器的操作，比如模拟点击图标
    //system("input tap 512 300");  // 屏幕中间点击打开文件管理器
    return INPUT_SWIPE(start, end, duration);
}
// 打开文件管理器并从中间向上滑动200像素
int ScreenTapDownToUp() {
    ad_point start = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};  // 屏幕中心
    ad_point end = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 300};  // 向上滑动200像素
    int duration = 300;  // 可调整滑动时长
    // 假设这里有打开文件管理器的操作，比如模拟点击图标
   // system("input tap start 300");  // 屏幕中间点击打开文件管理器
    return INPUT_SWIPE(start, end, duration);
}
// 打开控制面板并从上往下滑动200像素
int ScreenTapUpToDown() {
    ad_point start = {SCREEN_WIDTH / 2, 0};  // 屏幕顶部中间
    ad_point end = {SCREEN_WIDTH / 2, 200};  // 向下滑动200像素
    int duration = 300;  // 可调整滑动时长
    // 假设这里有打开控制面板的操作
    //system("input tap 512 50");  // 屏幕上方点击打开控制面板
    return INPUT_SWIPE(start, end, duration);
}

int TAP_SWIPE_DOWN() {
    ad_point start = {512 , 200};  //
    ad_point end = {512 , 500};    //
    int duration = 500;  // 可调整滑动时长
    return INPUT_SWIPE(start, end, duration);
}
int TAP_SWIPE_UP() {
    ad_point start = {512 , 500};  //
    ad_point end = {512 , 200};    //
    int duration = 500;  // 可调整滑动时长
    return INPUT_SWIPE(start, end, duration);
}

// 打开控制面板并从上往下滑动200像素
int OPEN_CONTROL_PANEL() {
    ad_point start = {SCREEN_WIDTH / 2, 0};  // 屏幕顶部中间
    ad_point end = {SCREEN_WIDTH / 2, 200};  // 向下滑动200像素
    int duration = 300;  // 可调整滑动时长
    // 假设这里有打开控制面板的操作
    //system("input tap 512 50");  // 屏幕上方点击打开控制面板
    return INPUT_SWIPE(start, end, duration);
}

// 从最左往最右滑动200像素
int SWIPE_LEFT_TO_RIGHT() {
    ad_point start = {0, SCREEN_HEIGHT / 2};  // 从最左侧开始，y值在屏幕中心
    ad_point end = {200, SCREEN_HEIGHT / 2};  // 向右滑动200像素
    int duration = 300;  // 可调整滑动时长
    return INPUT_SWIPE(start, end, duration);
}

// 从最右边滑到最左边200像素
int SWIPE_RIGHT_TO_LEFT() {
    ad_point start = {SCREEN_WIDTH, SCREEN_HEIGHT / 2};  // 从最右侧开始，y值在屏幕中心
    ad_point end = {SCREEN_WIDTH - 200, SCREEN_HEIGHT / 2};  // 向左滑动200像素
    int duration = 300;  // 可调整滑动时长
    return INPUT_SWIPE(start, end, duration);
}

int DEBUG_TAP_OPERATIONS() {
    // 1. 按下 Home 键
    printf("DEBUG: Pressing Home button...\n");
    INPUT_HOME();
    sleep(1);
    // 2. 打开文件管理器
    printf("DEBUG: Opening File Manager...\n");
    OPEN_FILE_MANAGER();
    sleep(1);

    // 3. 返回
    printf("DEBUG: Going Back...\n");
    INPUT_BACK();
    sleep(1);

    // 4. 打开控制面板
    printf("DEBUG: Opening Control Panel...\n");
    OPEN_CONTROL_PANEL();

    // 5. 返回
    printf("DEBUG: Going Back...\n");
    INPUT_BACK();
    sleep(1);

    // 6. 左滑
    printf("DEBUG: Swiping Left...\n");
    SWIPE_LEFT_TO_RIGHT();
    sleep(1);

    // 7. 右滑
    printf("DEBUG: Swiping Right...\n");
    SWIPE_RIGHT_TO_LEFT();
    sleep(1);

    // 结束调试
    printf("DEBUG completed\n");

    return 0;
}
