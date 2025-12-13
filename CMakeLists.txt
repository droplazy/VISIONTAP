cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# 设置工具链
set(NDK_STANDALONE_TOOLCHAIN /home/zhangjh/ndk-toolchains/arm64-v8a)
set(CMAKE_C_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang)
set(CMAKE_CXX_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang++)
set(CMAKE_FIND_ROOT_PATH ${NDK_STANDALONE_TOOLCHAIN})
add_definitions("--sysroot=${NDK_STANDALONE_TOOLCHAIN}/sysroot")

# OpenCV 配置
set(OpenCVHome /home/zhangjh/opencv/build-android/sdk_so/native)
include_directories(${OpenCVHome}/jni/include)
link_directories(${OpenCVHome}/libs/arm64-v8a)

# OpenSSL 配置
set(OPENSSL_ROOT_DIR /home/zhangjh/thirdlib_android/openssl/openssl-3.6.0/build)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
set(OPENSSL_LIBRARIES ${OPENSSL_ROOT_DIR}/lib)
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

# Mosquitto 配置
set(MOSQUITTO_ROOT_DIR /home/zhangjh/thirdlib_android/mqtt/mosquitto/build)
set(MOSQUITTO_INCLUDE_DIR ${MOSQUITTO_ROOT_DIR}/include)
set(MOSQUITTO_LIBRARIES ${MOSQUITTO_ROOT_DIR}/lib)
include_directories(${MOSQUITTO_INCLUDE_DIR})
link_directories(${MOSQUITTO_LIBRARIES})

# 链接库
link_libraries(
    # OpenCV 库
    opencv_imgcodecs
    opencv_imgproc
    opencv_highgui
    opencv_core
    opencv_video
    opencv_features2d
    opencv_videoio

    # OpenSSL 库
    ssl
    crypto

    # Mosquitto 库
    mosquitto

    # 系统库
    log
    z
)

# 可执行文件
add_executable(cv_exe
    ./main.cpp
    Thread_Tikok.cpp
    machine_vision.cpp
    screen_tap.cpp
    opencv_utils.cpp
    Thread_Tikok.h
    machine_vision.h
    public_def.h
    screen_tap.h
    opencv_utils.h
    mqtt_thread.h
    mqtt_thread.cpp
    json_utils.h
    json_utils.cpp
    ThreadBase.h
    ThreadBase.cpp
)
