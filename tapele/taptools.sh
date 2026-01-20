#!/bin/bash
#运行前先用 adb root 不然一些命令不能
echo "Received arguments: $@"


# 检查第一个参数并执行相应的操作
if [[ "$1" == "push" ]]; then
    echo "Running adb push command..."
    adb push /home/zhangjh/tap/build/Desktop_Qt_6_10_0-Debug/cv_exe /data/machine_vision

elif [[ "$1" == "snap" ]]; then
    echo "Running adb screencap command..."
    adb shell screencap -p /data/machine_vision/background.png
    adb pull /data/machine_vision/background.png .

elif [[ "$1" == "filepush" && -n "$2" ]]; then
    echo "Running adb push from $2 to /data/machine_vision/apppic..."
    adb push "./$2" "/data/machine_vision/apppic"
    
elif [[ "$1" == "cvdebug" ]]; then
    echo "Running adb pull debug_image_with_red_box.jpg..."
    adb pull /data/machine_vision/debug_image_with_red_box.jpg .

elif [[ "$1" == "package" ]]; then
    echo "打包应用程序..."
    
    # 复制 cv_exe 到指定目录
    echo "复制 cv_exe 到 /home/zhangjh/tap/tapele/upgrade_package/machine_vision ..."
    cp /home/zhangjh/tap/build/Desktop_Qt_6_10_0-Debug/cv_exe /home/zhangjh/tap/tapele/upgrade_package/machine_vision/
    
    # 进入目录并打包目录下的所有文件到根目录
    echo "打包 machine_vision 目录下的所有文件..."
    cd /home/zhangjh/tap/tapele/upgrade_package/machine_vision/
    tar -czf ../machine_vision.tar.gz *
    
    echo "打包完成！文件位于: /home/zhangjh/tap/tapele/upgrade_package/machine_vision.tar.gz"
    echo "包内结构：所有文件都在tar.gz的根目录，不包含machine_vision目录层级"
    
else
    echo "Usage:"
    echo "  $0 push             # 执行 adb push 操作"
    echo "  $0 snap             # 执行 adb screencap 操作"
    echo "  $0 filepush AAA /data/machine_vision/apppic # 执行文件 push 操作"
    echo "  $0 cvdebug          # 执行文件 拉取opencv 对比 操作"
    echo "  $0 package          # 打包应用程序到 upgrade_package 目录"
fi
