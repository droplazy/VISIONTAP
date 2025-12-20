#!/system/bin/sh

# 设置环境变量
export LD_LIBRARY_PATH=/data/machine_vision:$LD_LIBRARY_PATH

# 记录启动时间
echo "===== 机器视觉程序启动 ====="
echo "启动时间: $(date)"

# 创建 log 文件并写入验证信息
echo "this shell is waked！！！" > /data/machine_vision/mylog.txt

# 切换到程序目录并执行
cd /data/machine_vision 
./cv_exe 

# 记录退出状态
EXIT_CODE=$?
echo "程序退出，退出码: $EXIT_CODE"
echo "结束时间: $(date)"
echo "============================"

exit $EXIT_CODE

