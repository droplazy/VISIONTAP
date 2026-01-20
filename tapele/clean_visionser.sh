#!/system/bin/sh
setprop ctl.stop vendor.visionkeeper 2>/dev/null
pkill -9 visionkeeper 2>/dev/null
pkill -9 cv_exe 2>/dev/null
echo "" > /data/machine_vision/visionkeeper.log 2>/dev/null
ps -A | grep -E "visionkeeper|cv_exe" | grep -v grep || echo "清理完成"
