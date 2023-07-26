cmake_minimum_required(VERSION 3.4.1)
add_subdirectory(librtmp)
message("-------------------")
message(${CMAKE_BUILD_TYPE})
message(${ANDROID_PLATFORM})
message(${CMAKE_SOURCE_DIR})
message(${ANDROID_ABI})
file(GLOB cpp_source  *.cpp)
#头文件成功引入
include_directories(${CMAKE_SOURCE_DIR}/x264/${ANDROID_ABI}/include)
include_directories(${CMAKE_SOURCE_DIR}/faac/${ANDROID_ABI}/include)
#静态库 FFmpeg
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/x264/${ANDROID_ABI}/lib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/faac/${ANDROID_ABI}/lib")
add_library(
             native-lib
             SHARED
             ${cpp_source}
)

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                        log
                         rtmp
                        x264
			faac)