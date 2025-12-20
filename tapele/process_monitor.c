/**
 * VisionKeeper - Android进程监控守护进程
 * 编译命令:
 * /home/zhangjh/ndk-toolchains/arm64-v8a/bin/clang process_monitor.c -o visionkeeper -llog -O2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <sys/prctl.h>  // 添加prctl头文件
#include <android/log.h>

#define TAG "VisionKeeper"

// 进程相关信息
#define PROCESS_NAME "cv_exe"
#define LAUNCH_SCRIPT "/data/machine_vision/launch.sh"
#define LOG_FILE "/data/machine_vision/visionkeeper.log"
#define PID_FILE "/data/machine_vision/visionkeeper.pid"
#define CHECK_INTERVAL 3
#define MAX_RETRIES 3

// Android兼容的shell路径
#ifdef __ANDROID__
#define SHELL_PATH "/system/bin/sh"
#else
#define SHELL_PATH "/bin/sh"
#endif

// 简化日志函数 - 仅使用文件和控制台
void log_message(const char *format, ...) {
    char buffer[256];
    char timestamp[64];
    time_t now;
    struct tm *timeinfo;
    va_list args;
    
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // 输出到控制台
    printf("[%s] %s\n", timestamp, buffer);
    
    // 输出到文件
    FILE *log_fp = fopen(LOG_FILE, "a");
    if (log_fp != NULL) {
        fprintf(log_fp, "[%s] %s\n", timestamp, buffer);
        fclose(log_fp);
    }
}

// 检查进程是否在运行
int is_process_running(const char *process_name) {
    // 方法1: 使用pgrep
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "pgrep -x '%s' > /dev/null 2>&1", process_name);
    int ret = system(cmd);
    
    if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
        return 1;
    }
    
    // 方法2: 检查/proc目录
    DIR *dir;
    struct dirent *entry;
    
    dir = opendir("/proc");
    if (dir == NULL) {
        return 0;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char *endptr;
            long pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0' && pid > 0) {
                char path[256];
                char cmdline[256];
                
                snprintf(path, sizeof(path), "/proc/%ld/cmdline", pid);
                FILE *fp = fopen(path, "r");
                if (fp != NULL) {
                    if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
                        // 在Android中，cmdline可能包含完整路径
                        char *basename = strrchr(cmdline, '/');
                        if (basename) basename++;
                        else basename = cmdline;
                        
                        if (strstr(basename, process_name) != NULL) {
                            fclose(fp);
                            closedir(dir);
                            return 1;
                        }
                    }
                    fclose(fp);
                }
            }
        }
    }
    
    closedir(dir);
    return 0;
}

// 启动脚本
int launch_process() {
    pid_t pid;
    int status;
    
    log_message("开始启动脚本: %s", LAUNCH_SCRIPT);
    
    // 检查脚本权限
    if (access(LAUNCH_SCRIPT, X_OK) == -1) {
        log_message("警告: 尝试添加执行权限");
        chmod(LAUNCH_SCRIPT, 0755);
    }
    
    if (access(LAUNCH_SCRIPT, X_OK) == -1) {
        log_message("错误: 脚本没有执行权限: %s", strerror(errno));
        return -1;
    }
    
    pid = fork();
    if (pid < 0) {
        log_message("fork失败: %s", strerror(errno));
        return -1;
    }
    
    if (pid == 0) {
        // 子进程
        log_message("执行脚本: %s", LAUNCH_SCRIPT);
        
        // 重定向输出到日志文件
        int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
        if (fd >= 0) {
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        
        // 执行启动脚本
        if (execl(SHELL_PATH, "sh", LAUNCH_SCRIPT, (char *)NULL) == -1) {
            log_message("execl失败: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_SUCCESS);
    } else {
        // 父进程
        log_message("创建子进程成功，PID: %d", pid);
        
        // 等待脚本执行完成
        int timeout = 10;
        while (timeout-- > 0) {
            pid_t ret = waitpid(pid, &status, WNOHANG);
            if (ret == pid) {
                if (WIFEXITED(status)) {
                    int exit_code = WEXITSTATUS(status);
                    log_message("脚本执行完成，退出状态: %d", exit_code);
                    
                    if (exit_code == 127) {
                        log_message("错误127: shell或命令未找到，请检查SHELL_PATH");
                    }
                    
                    return exit_code;
                } else if (WIFSIGNALED(status)) {
                    log_message("脚本被信号终止: %d", WTERMSIG(status));
                    return -1;
                }
            } else if (ret == -1) {
                log_message("waitpid错误: %s", strerror(errno));
                return -1;
            }
            sleep(1);
        }
        
        log_message("脚本执行超时");
        return -1;
    }
}

// 信号处理函数
void signal_handler(int sig) {
    switch (sig) {
        case SIGTERM:
        case SIGINT:
            log_message("收到终止信号，退出守护进程");
            
            // 停止被监控的进程
            system("pkill -TERM cv_exe 2>/dev/null");
            
            // 删除PID文件
            unlink(PID_FILE);
            
            exit(EXIT_SUCCESS);
            break;
        case SIGHUP:
            log_message("收到SIGHUP信号");
            break;
    }
}

// 设置信号处理
void setup_signal_handlers() {
    struct sigaction sa;
    
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    
    // 忽略SIGPIPE
    signal(SIGPIPE, SIG_IGN);
}

// 守护进程初始化
int daemon_init() {
    pid_t pid;
    
    log_message("开始守护进程初始化");
    
    // 创建子进程
    pid = fork();
    if (pid < 0) {
        log_message("fork失败: %s", strerror(errno));
        return -1;
    }
    
    if (pid > 0) {
        // 父进程退出
        log_message("父进程退出，子进程PID: %d", pid);
        exit(EXIT_SUCCESS);
    }
    
    // 子进程继续执行
    log_message("守护进程PID: %d", getpid());
    
    // 创建新的会话
    if (setsid() < 0) {
        log_message("setsid失败: %s", strerror(errno));
        return -1;
    }
    
    // 更改工作目录到根目录
    if (chdir("/") < 0) {
        log_message("chdir失败: %s", strerror(errno));
        return -1;
    }
    
    // 重设文件权限掩码
    umask(0);
    
    // 设置进程名（可选）
    prctl(PR_SET_NAME, "VisionKeeper", 0, 0, 0);
    
    // 写入PID文件
    FILE *pid_fp = fopen(PID_FILE, "w");
    if (pid_fp != NULL) {
        fprintf(pid_fp, "%d\n", getpid());
        fclose(pid_fp);
    }
    
    // 关闭标准文件描述符
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // 重定向标准文件描述符到/dev/null
    int null_fd = open("/dev/null", O_RDWR);
    if (null_fd >= 0) {
        dup2(null_fd, STDIN_FILENO);
        dup2(null_fd, STDOUT_FILENO);
        dup2(null_fd, STDERR_FILENO);
        close(null_fd);
    }
    
    log_message("守护进程初始化完成");
    return 0;
}

int main(int argc, char *argv[]) {
    int retry_count = 0;
    int daemon_mode = 0;  // 0 = 调试模式，1 = 守护进程模式
    
    log_message("进程监控程序启动");
    log_message("监控进程: %s", PROCESS_NAME);
    log_message("启动脚本: %s", LAUNCH_SCRIPT);
    log_message("日志文件: %s", LOG_FILE);
    log_message("编译时间: %s %s", __DATE__, __TIME__);
    
    // 设置信号处理
    setup_signal_handlers();
    
    // 检查启动脚本是否存在
    if (access(LAUNCH_SCRIPT, F_OK) == -1) {
        log_message("错误: 启动脚本 %s 不存在", LAUNCH_SCRIPT);
        
        // 尝试创建示例脚本
        FILE *fp = fopen(LAUNCH_SCRIPT, "w");
        if (fp != NULL) {
            fprintf(fp, "#!/system/bin/sh\n");
            fprintf(fp, "# VisionKeeper自动生成的启动脚本\n");
            fprintf(fp, "echo '启动时间: $(date)'\n");
            fprintf(fp, "echo 'Android版本: $(getprop ro.build.version.release)'\n");
            fprintf(fp, "\n");
            fprintf(fp, "# 检查可执行文件\n");
            fprintf(fp, "CV_EXE='/data/machine_vision/cv_exe'\n");
            fprintf(fp, "if [ -x \"$CV_EXE\" ]; then\n");
            fprintf(fp, "    echo '启动: $CV_EXE'\n");
            fprintf(fp, "    nohup \"$CV_EXE\" > /data/local/tmp/cv_exe.log 2>&1 &\n");
            fprintf(fp, "    echo '新进程PID: $!'\n");
            fprintf(fp, "    exit 0\n");
            fprintf(fp, "else\n");
            fprintf(fp, "    echo '错误: $CV_EXE不存在或没有执行权限'\n");
            fprintf(fp, "    exit 1\n");
            fprintf(fp, "fi\n");
            fclose(fp);
            chmod(LAUNCH_SCRIPT, 0755);
            log_message("已创建示例启动脚本: %s", LAUNCH_SCRIPT);
        } else {
            log_message("无法创建启动脚本");
            return EXIT_FAILURE;
        }
    }
    
    // 检查启动脚本是否有执行权限
    if (access(LAUNCH_SCRIPT, X_OK) == -1) {
        log_message("错误: 启动脚本 %s 没有执行权限", LAUNCH_SCRIPT);
        return EXIT_FAILURE;
    }
    
    // 检查SHELL_PATH是否存在
    if (access(SHELL_PATH, X_OK) == -1) {
        log_message("警告: SHELL_PATH %s 不存在", SHELL_PATH);
        log_message("请修改代码中的SHELL_PATH定义");
    }
    
    /************************************************************
     * 调试模式 - 不成为守护进程
     * 要启用守护进程模式，修改下面的 #if 0 为 #if 1
     ************************************************************/
#if 0
    log_message("进入守护进程模式");
    if (daemon_init() < 0) {
        log_message("守护进程初始化失败");
        return EXIT_FAILURE;
    }
    daemon_mode = 1;
#else
    log_message("调试模式 - 不作为守护进程运行");
    log_message("按Ctrl+C退出程序");
#endif
    /************************************************************
     * 守护进程代码结束
     ************************************************************/
    
    // 主监控循环
    while (1) {
        if (!daemon_mode) {
            log_message("--------------------------------------------------");
        }
        
        // 检查进程是否在运行
        if (is_process_running(PROCESS_NAME)) {
            if (!daemon_mode) {
                log_message("进程 '%s' 正在运行，无需操作", PROCESS_NAME);
            }
            retry_count = 0;  // 重置重试计数
        } else {
            log_message("警告: 进程 '%s' 未在运行", PROCESS_NAME);
            
            // 延迟3秒
            log_message("延迟3秒后启动进程...");
            sleep(3);
            
            // 启动进程
            int result = launch_process();
            if (result == 0) {
                log_message("成功启动进程 '%s'", PROCESS_NAME);
                retry_count = 0;
            } else {
                log_message("启动进程失败，返回码: %d", result);
                retry_count++;
                
                if (retry_count >= MAX_RETRIES) {
                    log_message("错误: 达到最大重试次数 (%d)，等待更长时间后重试", MAX_RETRIES);
                    sleep(30);  // 等待30秒后重试
                    retry_count = MAX_RETRIES - 1;  // 重置但不归零
                }
            }
        }
        
        // 等待下一次检查
        sleep(CHECK_INTERVAL);
    }
    
    return EXIT_SUCCESS;
}
