# 项目名称

## 项目简介

## 文件说明

- `demo`: 使用动态链接库的示例代码。
  - `demo.cpp`:主函数入口。
- `include`:头文件目录。
  - `common.h`:通用头文件。
  - `*.h`:核心功能头文件。
  - `utils.h`:工具函数头文件。
  - `unittest_constant.h`:单元测试常量定义。
- `lib`:包含编译生成的二进制文件。
  - `libzipfiles.so`:生成的动态链接库文件。
- `src`:源代码文件目录。
  - `*.cpp`:源代码文件。
  - `utils.cpp`:工具函数实现。
- `tests`: 测试文件目录
  - `unittest/`:单元测试的测试代码目录。
  - `mock/`:单元测试测试时使用的目录。
  - `demo/`:示例代码运行时建议使用的目录。

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

## 概要设计

### 架构

```mermaid
graph LR
subgraph FRONTEND
    UI[图形界面]
    Terminal[命令行]
end
lib["libzipfiles.so"] --返回结果---> FRONTEND
FRONTEND --通过ffi调用传递参数---> lib["libzipfiles.so"]

stardardInterfaceStructure["标准接口结构
    {
        message: string,
        code: number(enum),
        data: any
    }
"]
```

> message & code 表示成功或者抛出的异常信息，data 表示返回的数据
> 用于将可能抛出异常的 cpp 函数封装成一个 C 风格的接口
> ps: 这是否是将一个 sum type 转换为了一个 product type

```mermaid
graph
subgraph BACKEND

end

BACKEND --根据绝对路径查找文件列表，根据条件筛选---> fileSystem
BACKEND --写入commit文件---> softwareDataPath
BACKEND --根据commit_id读取指定commit文件---> softwareDataPath

subgraph fileSystem[文件系统]

end

subgraph Software[软件]
        softwartInstallPath[软件数据路径: ~/.zip]
        subgraph softwareDataPath[软件备份路径: ~/.zip/backup]
            commit1["二进制文件 commit 5f..."]
            commit2["二进制文件 commit 8d..."]
            commit3["二进制文件 commit ac..."]
            commit4["二进制文件 commit 9e..."]
        end
        softwareLogPath["软件日志(file): ~/.zip/zflog.log"]
        softwareConfigPath["软件配置(file): ~/.zip/zfconfig.json"]
    end

FRONTEND --write ---> softwareLogPath
FRONTEND --write ---> softwareConfigPath

BACKEND --compile---> lib["libzipfiles.so"]
```

```mermaid
graph LR
subgraph BACKEND
    subgraph algorithm[算法]
        zip[压缩]
        unzip[解压]
        pack[打包]
        unpack[解包]
        encrypt[加密]
        decrypt[解密]
    end
    subgraph main[主模块]
    subgraph interface[接口]
        list["list({
            abs_path: fs::path
            type: number(enum)
            name: string
        }) -> vector<fs::path>"]
        commit["commit({
            message: string,
            files: vector< fs::path >
        }) -> void"]
        restore["restore({
            commit_id: number,
            files: vector< fs::path >
        }) -> void"]
    end
    subgraph interceptorList[拦截器链]
        zipInterceptor[压缩拦截器]
        packInterceptor[打包拦截器]
        encryptInterceptor[加密拦截器]
    end
    end

    list --请求文件列表---> fileSystem[文件系统]
    fileSystem --返回指定绝对路径下的文件并根据条件筛选---> list
    commit --提交文件信息---> packInterceptor
    packInterceptor --已打包文件---> zipInterceptor
    zipInterceptor --已压缩文件---> encryptInterceptor
    encryptInterceptor --已加密文件，写入指定路径---> fileSystem
    restore --获取指定commit_id的文件---> fileSystem
    fileSystem --返回指定commit_id的文件---> encryptInterceptor
    encryptInterceptor --已解密文件---> zipInterceptor
    zipInterceptor --已解压文件---> packInterceptor
    packInterceptor --已解包文件，写入指定路径---> fileSystem
end

BACKEND
```

## 打包解包设计

打包解包（10 分）：将所有备份文件拼接为一个大文件保存

git config --global --unset http.proxy
git config --global --unset https.proxy
libjson-glib-dev

linux mq 用于通信的结构体怎么编写，应该不能用指针吧，服务端的指针指向的位置客户端也无法访问  #define MAX_TEXT 512

struct message {
    long msg_type;
    char text[MAX_TEXT];
}; 是可以的  那

struct message {
    long msg_type;
    size_t count;
    char text[count];
};


sudo apt-get install lsb-release wget software-properties-common gnupg
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18
sudo apt-get update --fix-missing
sudo apt-get install clang-18
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100