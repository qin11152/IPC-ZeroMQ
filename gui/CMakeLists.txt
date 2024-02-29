cmake_minimum_required(VERSION 3.10)

project(testzmqClient)

#cmake编译时包括自身所在目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)
 
#开启qt需要的uic，moc，rcc
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#设定c++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
#指定qt所在目录
if(WIN32)
    set(CMAKE_PREFIX_PATH "d:/qt/5.9.6/mingw53_32") # Qt Kit Dir
elseif(UNIX)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES aarch64)
        set(CMAKE_PREFIX_PATH "/home/qin/app/qt/5.9.6/aarch64")
    else()
        set(CMAKE_PREFIX_PATH "/home/qin/app/qt/5.9.6/gcc_64")
    endif()

    # SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    # SET(CMAKE_INSTALL_RPATH "\${ORIGIN}:\${ORIGIN}/../lib")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)
find_package(Qt5 COMPONENTS Core Network Gui Widgets REQUIRED) 

include_directories(./module/IPC)
include_directories(./ui)

aux_source_directory(./ui srcs)
aux_source_directory(./module/IPC srcs)
aux_source_directory(./ srcs)

add_executable(${PROJECT_NAME} ${srcs})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets zmq pthread)