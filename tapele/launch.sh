#!/vendor/bin/sh
# launch.sh - 启动cv_exe进程

LOG_FILE="/data/vendor/cv_exe.log"
PID_FILE="/data/vendor/cv_exe.pid"
EXEC_PATH="/data/machine_vision/cv_exe"

echo "========================================" >> $LOG_FILE
echo "$(date): 启动cv_exe进程" >> $LOG_FILE
echo "PID: $$" >> $LOG_FILE
echo "PATH: $PATH" >> $LOG_FILE
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH" >> $LOG_FILE
echo "========================================" >> $LOG_FILE

# 检查可执行文件
if [ ! -x "$EXEC_PATH" ]; then
    echo "错误: $EXEC_PATH 不存在或没有执行权限" >> $LOG_FILE
    exit 127
fi

# 停止已存在的进程
if [ -f "$PID_FILE" ]; then
    OLD_PID=$(cat $PID_FILE)
    if [ -d "/proc/$OLD_PID" ]; then
        echo "停止旧进程: $OLD_PID" >> $LOG_FILE
        kill -TERM $OLD_PID 2>/dev/null
        sleep 1
        kill -KILL $OLD_PID 2>/dev/null
    fi
    rm -f $PID_FILE
fi

# 设置环境变量
export PATH=/system/bin:/vendor/bin:/system/xbin:$PATH
export LD_LIBRARY_PATH=/system/lib64:/vendor/lib64:/system/lib:/vendor/lib:$LD_LIBRARY_PATH
export ANDROID_DATA=/data
export ANDROID_ROOT=/system

# 启动进程
echo "执行: $EXEC_PATH" >> $LOG_FILE
nohup $EXEC_PATH >> $LOG_FILE 2>&1 &

# 记录PID
NEW_PID=$!
echo $NEW_PID > $PID_FILE
echo "新进程PID: $NEW_PID" >> $LOG_FILE

# 检查是否启动成功
sleep 2
if [ -d "/proc/$NEW_PID" ]; then
    echo "启动成功" >> $LOG_FILE
    exit 0
else
    echo "启动失败" >> $LOG_FILE
    exit 1
fi
