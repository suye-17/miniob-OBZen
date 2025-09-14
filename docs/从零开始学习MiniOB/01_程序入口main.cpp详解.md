# MiniOB程序入口详解 - main.cpp

## 🎯 学习目标

通过分析 `main.cpp`，你将理解：
1. C++程序的基本结构
2. 命令行参数处理
3. 系统初始化流程
4. 服务器启动过程

## 📁 文件位置
`src/observer/main.cpp` - 这是整个数据库服务器的入口点

## 🔍 代码结构分析

### 1. 头文件包含 (C++语法学习)

```cpp
#include <netinet/in.h>        // 网络编程相关
#include <unistd.h>           // Unix系统调用

#include "common/ini_setting.h"  // 配置文件处理
#include "common/init.h"        // 初始化函数
#include "net/server.h"         // 服务器类
```

**C++语法点：**
- `#include <...>` - 包含系统头文件
- `#include "..."` - 包含项目自定义头文件
- 头文件的作用：声明函数、类、常量等，让当前文件可以使用它们

### 2. 命名空间使用

```cpp
using namespace common;
```

**C++语法点：**
```cpp
// 不使用 using namespace 的情况：
common::cout << "Hello" << common::endl;

// 使用 using namespace 后：
cout << "Hello" << endl;  // 简洁多了！
```

### 3. 全局变量定义

```cpp
static Server *g_server = nullptr;
```

**C++语法学习：**
- `static` - 限制变量只在当前文件内可见
- `Server *` - 指向Server对象的指针
- `nullptr` - C++11的空指针，比老式的 `NULL` 更安全

### 4. 函数定义和调用

#### 4.1 usage() 函数 - 显示帮助信息
```cpp
void usage()
{
  cout << "Usage " << endl;
  cout << "-p: server port. if not specified, the item in the config file will be used" << endl;
  cout << "-f: path of config file." << endl;
  // ... 更多帮助信息
}
```

**作用：** 当用户输入错误参数时，显示正确的使用方法

#### 4.2 parse_parameter() 函数 - 解析命令行参数

```cpp
void parse_parameter(int argc, char **argv)
{
  string process_name = get_process_name(argv[0]);  // 获取程序名
  
  ProcessParam *process_param = the_process_param(); // 获取参数对象
  process_param->init_default(process_name);         // 初始化默认值
  
  // 解析命令行参数
  int opt;
  extern char *optarg;  // getopt函数使用的全局变量
  while ((opt = getopt(argc, argv, "dp:P:s:t:T:f:o:e:E:hn:")) > 0) {
    switch (opt) {
      case 's': process_param->set_unix_socket_path(optarg); break;
      case 'p': process_param->set_server_port(atoi(optarg)); break;
      case 'f': process_param->set_conf(optarg); break;
      // ... 更多参数处理
    }
  }
}
```

**C++语法学习：**

1. **函数参数：**
   ```cpp
   int argc        // 参数个数
   char **argv     // 参数数组，等价于 char* argv[]
   ```

2. **getopt函数：** Unix标准的命令行解析函数
   ```cpp
   // 命令行：./observer -p 8080 -f config.ini
   // argc = 5
   // argv[0] = "./observer"
   // argv[1] = "-p"
   // argv[2] = "8080"
   // argv[3] = "-f"  
   // argv[4] = "config.ini"
   ```

3. **while循环和switch语句：**
   ```cpp
   while ((opt = getopt(...)) > 0) {  // 循环处理每个参数
     switch (opt) {                   // 根据参数类型分别处理
       case 'p': /* 处理端口参数 */; break;
       case 'f': /* 处理配置文件 */; break;
       default: /* 未知参数 */; break;
     }
   }
   ```

#### 4.3 init_server() 函数 - 初始化服务器

```cpp
Server *init_server()
{
  // 1. 读取配置
  map<string, string> net_section = get_properties()->get(NET);
  ProcessParam *process_param = the_process_param();
  
  // 2. 设置默认值
  long listen_addr = INADDR_ANY;           // 监听所有网络接口
  long max_connection_num = MAX_CONNECTION_NUM_DEFAULT;  // 最大连接数
  int port = PORT_DEFAULT;                 // 默认端口
  
  
  
  // 4. 创建服务器参数对象
  ServerParam server_param;
  server_param.listen_addr = listen_addr;
  server_param.max_connection_num = max_connection_num;
  server_param.port = port;
  
  // 5. 根据协议类型设置参数
  if (0 == strcasecmp(process_param->get_protocol().c_str(), "mysql")) {
    server_param.protocol = CommunicateProtocol::MYSQL;
  } else if (0 == strcasecmp(process_param->get_protocol().c_str(), "cli")) {
    server_param.use_std_io = true;
    server_param.protocol = CommunicateProtocol::CLI;
  } else {
    server_param.protocol = CommunicateProtocol::PLAIN;
  }
  
  // 6. 创建并返回服务器对象
  Server *server = new Server(server_param);
  return server;
}
```

**C++语法学习：**

1. **map容器：**
   ```cpp
   map<string, string> net_section;        // 键值对容器
   map<string, string>::iterator it;       // 迭代器
   it = net_section.find(CLIENT_ADDRESS);  // 查找元素
   if (it != net_section.end()) {          // 判断是否找到
     string value = it->second;             // 获取值
   }
   ```

2. **字符串比较：**
   ```cpp
   strcasecmp(str1, str2)  // 不区分大小写的字符串比较
   // 返回0表示相等，<0表示str1<str2，>0表示str1>str2
   ```

3. **对象创建：**
   ```cpp
   Server *server = new Server(server_param);  // 在堆上创建对象
   // 注意：需要配对使用 delete server; 释放内存
   ```

## 🚀 main函数 - 程序主流程

```cpp
int main(int argc, char **argv)
{
  int rc = STATUS_SUCCESS;
  
  // 1. 显示启动信息
  cout << startup_tips;
  
  // 2. 设置信号处理（优雅退出）
  set_signal_handler(quit_signal_handle);
  
  // 3. 解析命令行参数
  parse_parameter(argc, argv);
  
  // 4. 系统初始化
  rc = init(the_process_param());
  if (rc != STATUS_SUCCESS) {
    cerr << "Shutdown due to failed to init!" << endl;
    cleanup();
    return rc;
  }
  
  // 5. 创建并启动服务器
  g_server = init_server();
  g_server->serve();        // 这里会阻塞，等待客户端连接
  
  // 6. 服务器停止后的清理工作
  LOG_INFO("Server stopped");
  cleanup();  1
  delete g_server;
  
  return 0;
}
```

### main函数流程图

```
程序启动
    ↓
显示启动信息
    ↓
设置信号处理器 (Ctrl+C优雅退出)
    ↓
解析命令行参数 (-p 端口, -f 配置文件等)
    ↓
系统初始化 (日志、配置、存储等)
    ↓
创建服务器对象
    ↓
启动服务器 (开始监听客户端连接)
    ↓
[服务器运行中，处理客户端请求]
    ↓
接收退出信号
    ↓
清理资源
    ↓
程序退出
```

## 🔧 信号处理机制

### 信号处理函数
```cpp
void quit_signal_handle(int signum)
{
  set_signal_handler(nullptr);    // 防止重复处理
  
  pthread_t tid;
  pthread_create(&tid, nullptr, quit_thread_func, (void *)(intptr_t)signum);
}

void *quit_thread_func(void *_signum)
{
  intptr_t signum = (intptr_t)_signum;
  LOG_INFO("Receive signal: %ld", signum);
  if (g_server) {
    g_server->shutdown();         // 优雅关闭服务器
  }
  return nullptr;
}
```

**作用：** 当用户按Ctrl+C时，不是立即强制退出，而是：
1. 接收信号
2. 创建新线程处理退出
3. 调用服务器的shutdown方法
4. 优雅地关闭所有连接和资源

**C++语法学习：**
- `pthread_create()` - 创建新线程
- `(void *)(intptr_t)signum` - 类型转换，将整数转为void指针

## 🎓 学习要点总结

### 1. C++程序结构
```cpp
// 头文件包含
#include <系统头文件>
#include "项目头文件"

// 命名空间
using namespace std;

// 全局变量
static Type *g_variable = nullptr;

// 函数定义
return_type function_name(parameters) {
    // 函数体
}

// 主函数
int main(int argc, char **argv) {
    // 程序主逻辑
    return 0;
}
```

### 2. 重要概念
- **命令行参数处理：** 让程序可以接受用户输入的参数
- **配置文件读取：** 从文件中读取程序配置
- **信号处理：** 处理系统信号，实现优雅退出
- **资源管理：** 创建资源后要记得清理

### 3. 程序启动流程
1. **参数解析** → 2. **系统初始化** → 3. **服务器创建** → 4. **服务启动** → 5. **运行服务** → 6. **优雅退出**

## 🔜 下一步学习

现在你已经理解了程序是如何启动的，接下来建议学习：
1. **Server类** - 理解服务器如何工作
2. **配置系统** - 理解程序如何读取配置
3. **日志系统** - 理解程序如何记录运行信息
4. **存储初始化** - 理解数据库如何初始化存储

这些都是数据库系统的基础组件！
