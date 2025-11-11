#include "machine_vision.h"
#include "screen_tap.h"

// 打开指定的应用
void turnon_application(AppType apptype)
{
    switch (apptype)
    {
    case APP_TIKTOK:
        std::cout << "DEBUG: Turning on TikTok..." << std::endl;
        TURN_ON_TIKTOK();
        break;

    case APP_WECHAT:
        std::cout << "DEBUG: Turning on WeChat..." << std::endl;
        TURN_ON_WECHAT();
        break;

    case APP_REDBOOK:
        std::cout << "DEBUG: Turning on RedBook..." << std::endl;
        TURN_ON_REDBOOK();
        break;

    case APP_MEITUAN:
        std::cout << "DEBUG: Turning on Meituan..." << std::endl;
        TURN_ON_MEITUAN();
        break;

    case APP_DAZHONGDIANPING:
        std::cout << "DEBUG: Turning on Da Zhong Dian Ping..." << std::endl;
        TURN_ON_DAZHONGDIANPING();
        break;

    case APP_BAIDU:
        std::cout << "DEBUG: Turning on Baidu..." << std::endl;
        TURN_ON_BAIDU();
        break;

    case APP_ALIPAY:
        std::cout << "DEBUG: Turning on Alipay..." << std::endl;
        TURN_ON_ALIPAY();
        break;

    case APP_QQ:
        std::cout << "DEBUG: Turning on QQ..." << std::endl;
        TURN_ON_QQ();
        break;

    case APP_KUAISHOU:
        std::cout << "DEBUG: Turning on Kuaishou..." << std::endl;
        TURN_ON_KUAISHOU();
        break;

    case APP_BILIBILI:
        std::cout << "DEBUG: Turning on Bilibili..." << std::endl;
        TURN_ON_BILIBILI();
        break;

    default:
        std::cout << "DEBUG: Unknown application" << std::endl;
        break;
    }
}



// 启动 TikTok
void TURN_ON_TIKTOK(void)
{
    // TODO: 填充启动 TikTok 的逻辑
    std::cout << "Launching TikTok..." << std::endl;
    INPUT_HOME();
    SHORT_DELAY;


}

// 启动 WeChat
void TURN_ON_WECHAT(void)
{
    // TODO: 填充启动 WeChat 的逻辑
    std::cout << "Launching WeChat..." << std::endl;
}

// 启动 RedBook
void TURN_ON_REDBOOK(void)
{
    // TODO: 填充启动 RedBook 的逻辑
    std::cout << "Launching RedBook..." << std::endl;
}

// 启动 Meituan
void TURN_ON_MEITUAN(void)
{
    // TODO: 填充启动 Meituan 的逻辑
    std::cout << "Launching Meituan..." << std::endl;
}

// 启动 大众点评
void TURN_ON_DAZHONGDIANPING(void)
{
    // TODO: 填充启动 大众点评 的逻辑
    std::cout << "Launching Da Zhong Dian Ping..." << std::endl;
}

// 启动 Baidu
void TURN_ON_BAIDU(void)
{
    // TODO: 填充启动 Baidu 的逻辑
    std::cout << "Launching Baidu..." << std::endl;
}

// 启动 Alipay
void TURN_ON_ALIPAY(void)
{
    // TODO: 填充启动 Alipay 的逻辑
    std::cout << "Launching Alipay..." << std::endl;
}

// 启动 QQ
void TURN_ON_QQ(void)
{
    // TODO: 填充启动 QQ 的逻辑
    std::cout << "Launching QQ..." << std::endl;
}

// 启动 Kuaishou
void TURN_ON_KUAISHOU(void)
{
    // TODO: 填充启动 Kuaishou 的逻辑
    std::cout << "Launching Kuaishou..." << std::endl;
}

// 启动 Bilibili
void TURN_ON_BILIBILI(void)
{
    // TODO: 填充启动 Bilibili 的逻辑
    std::cout << "Launching Bilibili..." << std::endl;
}
