cmake_minimum_required(VERSION 3.0)
project(zipfiles)

############################################
#### 项目配置
############################################

# 设置编译选项
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-O3 -DNDEBUG")
# 设置编译器为clang
set(CMAKE_CXX_COMPILER "clang++")
# 集成clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(ZIPFILES_HEADER_DIR ${CMAKE_SOURCE_DIR}/include) # 头文件目录
set(ZIPFILES_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src) # 源文件目录
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.1)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# 指定头文件搜索路径
include_directories(${GTK_INCLUDE_DIRS})
include_directories(${WEBKIT_INCLUDE_DIRS})
include_directories(${JSONCPP_INCLUDE_DIRS})
# 本项目搜索路径仅为include目录
include_directories(${ZIPFILES_HEADER_DIR})

############################################
#### 项目文件
############################################

# 全局
file(GLOB ZIPFILES_HEADER_FILES ${ZIPFILES_HEADER_DIR}/*.h)
file(GLOB ZIPFILES_SOURCE_FILES ${ZIPFILES_SOURCE_DIR}/*.cpp)
set(ZIPFILES_TEST_DIR ${CMAKE_SOURCE_DIR}/tests) # 测试目录
set(ZIPFILES_UNIT_TEST_DIR ${ZIPFILES_TEST_DIR}/unittest) # 单元测试目录
set(ZIPFILES_GUI_DIR ${CMAKE_SOURCE_DIR}/gui) # GUI 目录
set(ZIPFILES_DIST_DIR ${CMAKE_SOURCE_DIR}/gui/dist) # bundle 目录(将HTML、CSS和JavaScript文件打包在一起)
file(GLOB ZIPFILES_DIST_FILES ${ZIPFILES_DIST_DIR}/*)

# 客户端相关
set(ZIPFILES_CLIENT_HEADER_DIR ${ZIPFILES_HEADER_DIR}/client)
file(GLOB ZIPFILES_CLIENT_HEADER_FILES ${ZIPFILES_CLIENT_HEADER_DIR}/*.h)
set(ZIPFILES_CLIENT_SOURCE_DIR ${ZIPFILES_SOURCE_DIR}/client)
file(GLOB ZIPFILES_CLIENT_SOURCE_FILES ${ZIPFILES_CLIENT_SOURCE_DIR}/*.cpp)
# 客户端暂时不需要单元测试

# 服务端相关
set(ZIPFILES_SERVER_HEADER_DIR ${ZIPFILES_HEADER_DIR}/server)
file(GLOB ZIPFILES_SERVER_HEADER_FILES ${ZIPFILES_SERVER_HEADER_DIR}/*.h)
set(ZIPFILES_SERVER_SOURCE_DIR ${ZIPFILES_SOURCE_DIR}/server)
file(GLOB ZIPFILES_SERVER_SOURCE_FILES ${ZIPFILES_SERVER_SOURCE_DIR}/*.cpp)
set(ZIPFILES_SERVER_UNIT_TEST_DIR ${ZIPFILES_UNIT_TEST_DIR}/server)
file(GLOB ZIPFILES_SERVER_UNIT_TEST_FILES ${ZIPFILES_SERVER_UNIT_TEST_DIR}/*.cpp)

# 中台相关
set(ZIPFILES_MP_HEADER_DIR ${ZIPFILES_HEADER_DIR}/mp)
file(GLOB ZIPFILES_MP_HEADER_FILES ${ZIPFILES_MP_HEADER_DIR}/*.h)
set(ZIPFILES_MP_SOURCE_DIR ${ZIPFILES_SOURCE_DIR}/mp)
file(GLOB ZIPFILES_MP_SOURCE_FILES ${ZIPFILES_MP_SOURCE_DIR}/*.cpp)


############################################
#### 项目打包
############################################

# 服务端打包
add_executable(
    server
    ${ZIPFILES_SOURCE_FILES}
    ${ZIPFILES_MP_SOURCE_FILES}
    ${ZIPFILES_SERVER_SOURCE_FILES}
)

target_link_libraries(
    server
    ${JSONCPP_LIBRARIES}
)

# 客户端打包

add_executable(
    client
    ${ZIPFILES_SOURCE_FILES}
    ${ZIPFILES_CLIENT_SOURCE_FILES}
    ${ZIPFILES_MP_SOURCE_FILES}
)
target_link_libraries(
    client
    ${GTK_LIBRARIES}
    ${WEBKIT_LIBRARIES}
    ${JSONCPP_LIBRARIES}
)
target_compile_options(client PRIVATE ${GTK_CFLAGS_OTHER} ${WEBKIT_CFLAGS_OTHER})

install(DIRECTORY ${ZIPFILES_DIST_DIR} DESTINATION ${CMAKE_BINARY_DIR})

add_custom_command(
    TARGET client
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ZIPFILES_DIST_DIR} ${CMAKE_BINARY_DIR}/dist
    COMMENT "Running install after building client"
)

############################################
#### 项目测试
############################################

find_package(GTest REQUIRED)
# 单元测试
enable_testing()

foreach(TEST_FILE ${ZIPFILES_SERVER_UNIT_TEST_FILES})
    # 获取文件名（不带路径和扩展名）+ 服务端单元测试
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # 创建测试目标
    add_executable(${TEST_NAME} ${TEST_FILE} ${ZIPFILES_SOURCE_FILES} ${ZIPFILES_SERVER_SOURCE_DIR}/${TEST_NAME}.cpp)

    # 链接 Google Test 库
    target_link_libraries(${TEST_NAME} GTest::GTest GTest::Main)

    # 添加测试到 CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 10)
endforeach()



############################################
#### 工具
############################################

# 源码格式化
add_custom_target(
    CPP_FORMAT
    COMMAND clang-format
    -i
    -style=file
    ${ZIPFILES_HEADER_FILES}
    ${ZIPFILES_SOURCE_FILES}
    ${ZIPFILES_CLIENT_HEADER_FILES}
    ${ZIPFILES_CLIENT_SOURCE_FILES}
    ${ZIPFILES_SERVER_HEADER_FILES}
    ${ZIPFILES_SERVER_SOURCE_FILES}
    ${ZIPFILES_SERVER_UNIT_TEST_FILES}
    ${ZIPFILES_MP_HEADER_FILES}
    ${ZIPFILES_MP_SOURCE_FILES}
    COMMENT "Formatting source files"
)

# 计算代码行数
add_custom_target(
    LOC
    COMMAND find
    ${ZIPFILES_HEADER_DIR}
    ${ZIPFILES_SOURCE_DIR}
    ${ZIPFILES_CLIENT_HEADER_DIR}
    ${ZIPFILES_CLIENT_SOURCE_DIR}
    ${ZIPFILES_SERVER_HEADER_DIR}
    ${ZIPFILES_SERVER_SOURCE_DIR}
    ${ZIPFILES_SERVER_UNIT_TEST_DIR}
    ${ZIPFILES_MP_HEADER_DIR}
    ${ZIPFILES_MP_SOURCE_DIR}
    -name "*.h" -o -name "*.cpp" | xargs wc -l
    COMMENT "Counting lines of code"
)

