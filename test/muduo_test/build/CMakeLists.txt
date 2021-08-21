cmake_minimum_required(VERSION 3.0) # 最低版本
project(main)   # 给这个工程一个名字，这不是可执行文件的名字，是工程的名字

# 配置编译选项
# set(CMAKE_CXX_FLAGS $(CMAKE_CXX_FLAGS) -g)  # 添加编译选项
# 我这里一 -g 就不行，目前还不知道为啥

# 配置头文件搜索路径
# include_directories('如果想在默认路径之外再加路径，就填在这里')

# 配置库文件搜索路径
# link_directories('意思同上')

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 设置需要编译的源文件列表
set(SRC_LIST muduo_server_test.cpp)  # 可放多个，但是没必要

# 把 . 指定路径下的所有源文件放入一个变量名中
# aux_source_directory(. SRC_LIST)

# 配置可执行文件名
add_executable(muduo_test ${SRC_LIST})
 
#生成库
# add_library('库名')

# 需要链接的外库
target_link_libraries(muduo_test muduo_net muduo_base pthread)   # 如果库之间有相互依赖，注意把被依赖的放前面

