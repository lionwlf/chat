cmake_minimum_required(VERSION 3.0) # 最低版本
project(main)   # 给这个工程一个名字，这不是可执行文件的名字，是工程的名字

# 设置编译选项，不知道最后能不能过
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 设置可执行文件最后的输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

# 加载子目录
add_subdirectory(src)