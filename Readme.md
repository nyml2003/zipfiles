# 项目名称

## 项目简介

## 文件说明
- `bin`:包含编译生成的二进制文件。
  - `libzipfiles.so`:生成的共享库文件。
- `include`:头文件目录。
  - `common.h`:通用头文件。
  - `*.h`:核心功能头文件。
  - `utils.h`:工具函数头文件。
- `src`:源代码文件目录。
  - `*.cpp`:源代码文件。
  - `utils.cpp`:工具函数实现。
- `tests`: 测试文件目录
  - `unittest/`:单元测试目录。
- `demo`: 使用动态链接库的示例代码。
  - `main.cpp`:主函数入口。


## 命名空间
- `zipfiles`:主要命名空间。

## 使用方法
TODO 待完善
### 构建项目
```sh
mkdir build
cd build
cmake ..
make
```