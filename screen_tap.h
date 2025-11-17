#ifndef SCREEN_TAP_H   // 判断是否已经定义过 MACHINE_VISION_H
#define SCREEN_TAP_H   // 如果没有定义过，则定义它

#include "pulic_def.h"


int INPUT_TAP(ad_point);
int INPUT_SWIPE(ad_point start, ad_point end, int duration);
int INPUT_HOME();
int INPUT_BACK();
int INPUT_TAP_DELAY(ad_point pos,int delay );

int OPEN_FILE_MANAGER();                      // 打开文件管理器
int OPEN_CONTROL_PANEL();                     // 打开控制面板
int SWIPE_LEFT_TO_RIGHT();                    // 从左到右滑动200像素
int SWIPE_RIGHT_TO_LEFT();                    // 从右到左滑动200像素
int DEBUG_TAP_OPERATIONS();


int ScreenTapUpToDown();
int ScreenTapDownToUp();
#endif
