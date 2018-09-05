### **Duilib_Ultimate**

*** 
此项目在原项目基础上，对vs2017环境下编译做了优化，解决几乎所有的编译警告。

在vs2017文件夹里，新建项目，引入头文件及对应lib文件即可解决引入问题。

#### 如果需要引入动态库，则添加下列代码：

```C++
#if (defined _UNICODE) && (defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_64d.lib")
#elif (defined _UNICODE) && (defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_d.lib")
#elif (defined _UNICODE) && (!defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_64.lib")
#elif (defined _UNICODE) && (!defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib.lib")
#elif (!defined _UNICODE) && (defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_64d.lib")
#elif (!defined _UNICODE) && (defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_d.lib")
#elif (!defined _UNICODE) && (!defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_64.lib")
#elif (!defined _UNICODE) && (!defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA.lib")
#endif
```

#### 如果需要引入静态库，则添加下列代码：

```C++
#if (defined _UNICODE) && (defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_64sd.lib")
#elif (defined _UNICODE) && (defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_sd.lib")
#elif (defined _UNICODE) && (!defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_64s.lib")
#elif (defined _UNICODE) && (!defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLib_s.lib")
#elif (!defined _UNICODE) && (defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_64sd.lib")
#elif (!defined _UNICODE) && (defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_sd.lib")
#elif (!defined _UNICODE) && (!defined _DEBUG) && (defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA_64s.lib")
#elif (!defined _UNICODE) && (!defined _DEBUG) && (!defined _WIN64)
#	pragma comment (lib, "../lib/DuiLibA.lib")
#endif
```

然后在项目的预处理器加上宏定义“UILIB_STATIC”，再然后将编译选项分别设置为/MT、/MTd即可。
