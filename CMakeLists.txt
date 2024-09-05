cmake_minimum_required(VERSION 3.0)
project(zipfiles)

# 设置编译选项
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-O3 -DNDEBUG")

# 设置目录
set(ZIPFILES_HEADER_DIR ${CMAKE_SOURCE_DIR}/include)
set(ZIPFILES_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(ZIPFILES_TEST_DIR ${CMAKE_SOURCE_DIR}/tests)
set(ZIPFILES_DEMO_DIR ${CMAKE_SOURCE_DIR}/demo)
set(ZIPFILES_UNIT_TEST_DIR ${ZIPFILES_TEST_DIR}/unittest)
set(ZIPFILES_LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ZIPFILES_LIB_DIR})
find_package(GTest REQUIRED)

# 指定头文件搜索路径
include_directories(${ZIPFILES_HEADER_DIR})

# 设置源文件
file(GLOB ZIPFILES_UNIT_TEST_FILES ${ZIPFILES_UNIT_TEST_DIR}/*.cpp)
file(GLOB ZIPFILES_SOURCE_FILES ${ZIPFILES_SOURCE_DIR}/*.cpp)
file(GLOB ZIPFILES_HEADER_FILES ${ZIPFILES_HEADER_DIR}/*.h)
set(ZIPFILES_DEMO_ENTRY_FILE ${ZIPFILES_DEMO_DIR}/demo.cpp)

# 源码格式化
add_custom_target(
    CPP_FORMAT
    COMMAND clang-format -i -style=file ${ZIPFILES_SOURCE_FILES} ${ZIPFILES_HEADER_FILES} ${ZIPFILES_UNIT_TEST_FILES}
    COMMENT "Formatting source files"
)

# 生成动态链接库
add_library(ZIPFILES_SHARED SHARED ${ZIPFILES_SOURCE_FILES})
set_target_properties(ZIPFILES_SHARED PROPERTIES OUTPUT_NAME "zipfiles")
set_target_properties(ZIPFILES_SHARED PROPERTIES POSITION_INDEPENDENT_CODE ON)

# 示例程序
add_executable(
    ZIPFILES_DEMO
    ${ZIPFILES_DEMO_ENTRY_FILE}
)
add_dependencies(ZIPFILES_DEMO CPP_FORMAT)
target_link_libraries(ZIPFILES_DEMO ZIPFILES_SHARED)

# 单元测试
enable_testing()

foreach(TEST_FILE ${ZIPFILES_UNIT_TEST_FILES})
    # 获取文件名（不带路径和扩展名）
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # 创建测试目标
    add_executable(${TEST_NAME} ${ZIPFILES_SOURCE_FILES} ${TEST_FILE})

    # 链接 Google Test 库
    target_link_libraries(${TEST_NAME} GTest::GTest GTest::Main)

    # 添加测试到 CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 10)
endforeach()
