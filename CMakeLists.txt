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
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(ZIPFILES_HEADER_DIR ${CMAKE_SOURCE_DIR}/include) # 头文件目录
set(ZIPFILES_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src) # 源文件目录
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(WEBKIT REQUIRED webkit2gtk-4.1)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
pkg_check_modules(LOG4CPP REQUIRED log4cpp)
pkg_check_modules(CRYPTOPP REQUIRED libcrypto++)


# 指定头文件搜索路径
include_directories(${GTK_INCLUDE_DIRS})
include_directories(${WEBKIT_INCLUDE_DIRS})
include_directories(${JSONCPP_INCLUDE_DIRS})
include_directories(${LOG4CPP_INCLUDE_DIRS})
include_directories(${CRYPTOPP_INCLUDE_DIRS})
# 本项目搜索路径仅为include目录
include_directories(${ZIPFILES_HEADER_DIR})

############################################
#### 项目文件
############################################

# 全局
set(ZIPFILES_TEST_DIR ${CMAKE_SOURCE_DIR}/tests) # 测试目录
set(ZIPFILES_UNIT_TEST_DIR ${ZIPFILES_TEST_DIR}/unittest) # 单元测试目录
set(ZIPFILES_DIST_DIR ${CMAKE_SOURCE_DIR}/gui/dist)
set(ZIPFILES_CONFIG_DIR ${CMAKE_SOURCE_DIR}/config)
set(ZIPFILES_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin)
set(ZIPFILES_FRONTEND_DIR ${CMAKE_SOURCE_DIR}/gui/src)

# 客户端相关
set(ZIPFILES_CLIENT_HEADER_DIR ${ZIPFILES_HEADER_DIR}/client)
file(GLOB_RECURSE ZIPFILES_CLIENT_HEADER_FILES ${ZIPFILES_CLIENT_HEADER_DIR}/*.h)
set(ZIPFILES_CLIENT_SOURCE_DIR ${ZIPFILES_SOURCE_DIR}/client)
file(GLOB_RECURSE ZIPFILES_CLIENT_SOURCE_FILES ${ZIPFILES_CLIENT_SOURCE_DIR}/*.cpp)
set(ZIPFILES_CLIENT_LOG_CONFIG ${ZIPFILES_CONFIG_DIR}/client.log.properties)
# 客户端暂时不需要单元测试

# 服务端相关
set(ZIPFILES_SERVER_HEADER_DIR ${ZIPFILES_HEADER_DIR}/server)
file(GLOB_RECURSE ZIPFILES_SERVER_HEADER_FILES ${ZIPFILES_SERVER_HEADER_DIR}/*.h)
set(ZIPFILES_SERVER_SOURCE_DIR ${ZIPFILES_SOURCE_DIR}/server)
# 递归地去查找所有的.cpp文件
file(GLOB_RECURSE ZIPFILES_SERVER_SOURCE_FILES ${ZIPFILES_SERVER_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM ZIPFILES_SERVER_SOURCE_FILES ${EXCLUDE_FILE})
# 服务端测试目录
set(ZIPFILES_SERVER_UNIT_TEST_DIR ${ZIPFILES_UNIT_TEST_DIR}/server)
file(GLOB_RECURSE ZIPFILES_SERVER_UNIT_TEST_FILES ${ZIPFILES_SERVER_UNIT_TEST_DIR}/*.cpp)
# 服务端配置目录
set(ZIPFILES_SERVER_LOG_CONFIG ${ZIPFILES_CONFIG_DIR}/server.log.properties)

# 中台相关
set(ZIPFILES_MP_HEADER_DIR ${ZIPFILES_HEADER_DIR}/mp)
file(GLOB_RECURSE ZIPFILES_MP_HEADER_FILES ${ZIPFILES_MP_HEADER_DIR}/*.h)
set(ZIPFILES_MP_SOURCE_DIR ${ZIPFILES_SOURCE_DIR}/mp)
file(GLOB_RECURSE ZIPFILES_MP_SOURCE_FILES ${ZIPFILES_MP_SOURCE_DIR}/*.cpp)

# 可执行文件入口
set(ZIPFILES_ENTRY_DIR ${ZIPFILES_SOURCE_DIR}/entry)
set(ZIPFILES_CLINET_ENTRY ${ZIPFILES_ENTRY_DIR}/client.cpp)
set(ZIPFILES_SERVER_ENTRY ${ZIPFILES_ENTRY_DIR}/server.cpp)
set(ZIPFILES_MOCK_SERVER_ENTRY ${ZIPFILES_TEST_DIR}/mock/mock.cpp)


############################################
#### 项目打包
############################################

# 服务端打包
add_executable(
    server
    ${ZIPFILES_MP_SOURCE_FILES}
    ${ZIPFILES_SERVER_SOURCE_FILES}
    ${ZIPFILES_SERVER_ENTRY}
)

set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ZIPFILES_OUTPUT_DIR})

install(FILES ${ZIPFILES_SERVER_LOG_CONFIG} DESTINATION ${ZIPFILES_OUTPUT_DIR})

target_link_libraries(
    server
    ${JSONCPP_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
    ${CRYPTOPP_LIBRARIES}
)

# 客户端打包

add_executable(
    client
    ${ZIPFILES_MP_SOURCE_FILES}
    ${ZIPFILES_CLIENT_SOURCE_FILES}
    ${ZIPFILES_CLINET_ENTRY}
)

set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ZIPFILES_OUTPUT_DIR})

target_link_libraries(
    client
    ${GTK_LIBRARIES}
    ${WEBKIT_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)
target_compile_options(client PRIVATE ${GTK_CFLAGS_OTHER} ${WEBKIT_CFLAGS_OTHER})

install(DIRECTORY ${ZIPFILES_DIST_DIR} DESTINATION ${ZIPFILES_OUTPUT_DIR})
install(FILES ${ZIPFILES_CLIENT_LOG_CONFIG} DESTINATION ${ZIPFILES_OUTPUT_DIR})

############################################
#### 项目测试
############################################


# 模拟给后端发请求，算集成测试（？）

add_executable(
    mock
    ${ZIPFILES_MOCK_SERVER_ENTRY}
    ${ZIPFILES_MP_SOURCE_FILES}

)

target_link_libraries(
    mock
    ${JSONCPP_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)




find_package(GTest REQUIRED)
# 单元测试
enable_testing()

foreach(TEST_FILE ${ZIPFILES_SERVER_UNIT_TEST_FILES})
    # 获取文件名（不带路径和扩展名）+ 服务端单元测试
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # 创建测试目标
    add_executable(${TEST_NAME} ${TEST_FILE} ${ZIPFILES_SERVER_SOURCE_FILES} ${ZIPFILES_MP_SOURCE_FILES})

    # 链接 Google Test 库
    target_link_libraries(${TEST_NAME} GTest::GTest GTest::Main ${LOG4CPP_LIBRARIES} ${JSONCPP_LIBRARIES} ${CRYPTOPP_LIBRARIES})

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
    COMMAND clang-format -i -style=file -assume-filename=${CMAKE_SOURCE_DIR}/.clang-format ${ZIPFILES_HEADER_FILES} $ ${ZIPFILES_CLIENT_SOURCE_FILES} ${ZIPFILES_MP_SOURCE_FILES} ${ZIPFILES_SERVER_SOURCE_FILES} ${ZIPFILES_SERVER_UNIT_TEST_FILES} ${ZIPFILES_MOCK_SERVER_ENTRY}
    COMMENT "Formatting source files"
)


# 计算代码行数
add_custom_target(
    LOC
    COMMAND find
    ${ZIPFILES_HEADER_DIR}
    ${ZIPFILES_SOURCE_DIR}
    ${ZIPFILES_FRONTEND_DIR}
    -type f -name "*.h" -o -name "*.cpp" -o -name "*.ts" -o -name "*.tsx" | xargs wc -l
    COMMENT "Counting lines of code"
)

find_program(CLANGD clangd)

if(CLANGD)
    add_custom_target(RUN_CLANGD
        COMMAND ${CLANGD} --compile-commands-dir=${CMAKE_BINARY_DIR} ${ZIPFILES_HEADER_FILES} ${ZIPFILES_HEADER_FILES}
        COMMENT "Running clangd and collecting errors"
    )
else()
    message(WARNING "clangd not found, skipping RUN_CLANGD target")
endif()



