cmake_minimum_required(VERSION 2.8.3)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(OpenCVHome /home/zhangjh/opencv/build-android/sdk_so/native)
set(NDK_STANDALONE_TOOLCHAIN /home/zhangjh/ndk-toolchains/arm64-v8a)
set(CMAKE_C_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang)
set(CMAKE_CXX_COMPILER ${NDK_STANDALONE_TOOLCHAIN}/bin/clang++)
set(CMAKE_FIND_ROOT_PATH ${NDK_STANDALONE_TOOLCHAIN})
add_definitions("--sysroot=${NDK_STANDALONE_TOOLCHAIN}/sysroot")

# target_sources(tap
#   PRIVATE

#     app_tiktok.cpp app_tiktok.h CMakeLists.txt machine_vision.cpp machine_vision.h main.cpp pulic_def.h screen_tap.cpp screen_tap.h
# )


include_directories(${OpenCVHome}/jni/include)
link_directories(${OpenCVHome}/libs/arm64-v8a)
               #  ${OpenCVHome}/3rdparty/libs/arm64-v8a)

link_libraries(
        opencv_imgcodecs
        opencv_imgproc
        opencv_highgui
        opencv_core
        opencv_video
        opencv_features2d
        opencv_videoio
     #   cpufeatures
     #   tegra_hal
     #   IlmImf
    #    libjasper
    #    libjpeg-turbo
    #    libpng
     #   libprotobuf
    #    libtiff
    #    libwebp
    #    quirc
        log
        z
        )
add_executable(cv_exe ./main.cpp app_tiktok.cpp machine_vision.cpp screen_tap.cpp)
                                                         
