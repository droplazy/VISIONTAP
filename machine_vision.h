#ifndef MACHINE_VISION_H   // 判断是否已经定义过 MACHINE_VISION_H
#define MACHINE_VISION_H   // 如果没有定义过，则定义它

#include "pulic_def.h"
#include "opencv_utils.h"
#include <fstream>


#define FILE_SYSTEM_TEXT (120,357)


#define TEXTURE_CLICK {31, 107}

// 宏定义文件路径
#define FILESYSTEM_PATH            "/data/machine_vision/apppic/filesystem.png"
#define PIPETXT_PATH               "/data/machine_vision/apppic/pipetxt.png"
#define PIPETXT_LOCATE_PATH               "/data/machine_vision/apppic/pipefile.png"

#define TEXTCONTENT_PATH           "/data/machine_vision/apppic/textcontent.png"
#define ALLSELECT_PATH             "/data/machine_vision/apppic/allselect.png"
#define TIKTOK_PATH                "/data/machine_vision/apppic/tiktok.png"
#define TEXTCOPY_PATH              "/data/machine_vision/apppic/tetxcopy.png"
#define DELFILE_PATH               "/data/machine_vision/apppic/delfile.png"
#define DELFILECERTAIN_PATH        "/data/machine_vision/apppic/delcertain.png"
#define TEXTPASTE_PATH             "/data/machine_vision/apppic/TextPaste.png"


// 声明一个测试函数
void test_function();
void turnon_application(AppType apptype);  // 启动指定的应用

// 启动具体应用的函数声明
void TURN_ON_TIKTOK(void);          // 启动 TikTok
void TURN_ON_WECHAT(void);          // 启动 WeChat
void TURN_ON_REDBOOK(void);         // 启动 RedBook
void TURN_ON_MEITUAN(void);         // 启动 Meituan
void TURN_ON_DAZHONGDIANPING(void); // 启动 大众点评
void TURN_ON_BAIDU(void);           // 启动 Baidu
void TURN_ON_ALIPAY(void);          // 启动 Alipay
void TURN_ON_QQ(void);              // 启动 QQ
void TURN_ON_KUAISHOU(void);        // 启动 Kuaishou
void TURN_ON_BILIBILI(void);        // 启动 Bilibili
void INPUT_TYPEINGTEXT(string text);
// // JNI 方法：复制文本到剪贴板
// void copyToClipboard(const std::string& text);
// // JNI 方法：从剪贴板获取文本
// std::string getFromClipboard();
int CopyTextFormSys(string texture);
int FindTargetClick(string targetPng, bool clickdelay); //clickdelay=1 重点击  否则轻量点击
ad_point FindTargetReturnPoint(string targetPng);//返回目标位置
int checkFileExistsWithTimeout(const char* filePath, int timeoutSeconds) ;
ad_point FindTargetForDelay(string targetPng,double &score,int clycles);
#endif // 结束 #ifndef
