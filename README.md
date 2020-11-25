# hikvisionCameraAPI

> 海康威视工业相机驱动程序封装接口

项目说明：由于海康威视MVS自带的DEMO中对相机图像的获取过于繁琐，所以我对驱动程序进行了二次封装！只需要调用两个初始化函数就会自动初始化好相机，接着调用获取图像接口就可以不断读取缓冲池中的图像了！

## 文件结构

```file
+--- hikvisonCameraAPI  #项目根目录
    +--- 64 #相机驱动程序依赖库环境
    +--- DEMO   #驱动使用样例程序
        --- cameraTest.cpp  #测试程序源文件
        --- Makefile    #编译规则
    +--- include    #官方原生相机接口头文件
    ---- cameraGetImage.h   #封装后的用户API头文件
    ---- cameraGetImageSource.cpp   #封装源程序
    ---- README.md  #工程说明文件
```

## 使用说明ATTENTION

- 该项目默认获取的图像格式为：RGB
- 封装接口语言为：C
- 适用平台为Linux
- 在使用该封装前，请先安装官方MVS环境
- 运行DEMO中的示例文件
    1. 打开终端
    2. 运行$ make
    3. 运行$ ./cameraTest
