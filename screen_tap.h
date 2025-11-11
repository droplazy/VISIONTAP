#ifndef SCREEN_TAP_H   // 判断是否已经定义过 MACHINE_VISION_H
#define SCREEN_TAP_H   // 如果没有定义过，则定义它

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} ad_point;

int INPUT_TAP(int x, int y);
int INPUT_SWIPE(ad_point start, ad_point end, int duration);
int INPUT_HOME();
int INPUT_BACK();

int OPEN_FILE_MANAGER();                      // 打开文件管理器并向上滑动200像素
int OPEN_CONTROL_PANEL();                     // 打开控制面板并向下滑动200像素
int SWIPE_LEFT_TO_RIGHT();                    // 从左到右滑动200像素
int SWIPE_RIGHT_TO_LEFT();                    // 从右到左滑动200像素
int DEBUG_TAP_OPERATIONS();

#endif