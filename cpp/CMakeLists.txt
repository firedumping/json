cmake_minimum_required(VERSION 3.20)
project(hello LANGUAGES CXX)

#创建静态库 jsonlite
add_library(jsonlite STATIC 
    src/JsonLite.cpp)

#使用 jsonlite的 代码可以包括 include/下的头文件
target_include_directories(jsonlite
    PUBLIC $(CMAKE_CURRENT_SOURCE_DIR)/include)
#PUBLIC 表示这个设置同时用于 demo_math 自身和它的使用者。


#创建可执行程序 hello
add_executable(hello cli/main.cpp)

# hello 依赖 jsonlite
target_link_libraries(hello 
    PRIVATE jsonlite)