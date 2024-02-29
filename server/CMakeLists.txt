cmake_minimum_required(VERSION 3.15)

project(server)

include_directories(./include)

aux_source_directory(./ srcs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

add_executable(${PROJECT_NAME} ${srcs})

target_link_libraries(${PROJECT_NAME} zmq pthread)