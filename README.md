Qt http服务器示例

http服务：httplib
json解析：nlohmann/json

工程配置文件要点：
```
QT       += core gui network

win32: LIBS += -lSetupAPI -luser32 -lws2_32

```

当前功能：
* 监听8080端口。

* 实现get接口：/version

* 实现post接口：/foobar

  

测试环境：qt5.14.2+mingw64 