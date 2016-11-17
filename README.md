## eSDK\_VCN\_API\_Windows_CPP  ##
华为VCN3000集实时监控、录像检索、录像回放、云镜控制、告警联动、语音对讲等多种视频监控业务于一身，并可配套IVS转码服务器提供移动监控业务，为政府、交通、教育部门及不同规模的企业客户构建安全、便捷、实用的系列化视频监控产品解决方案。

## 版本更新 ##
eSDK IVS最新版本v2.1.00.100

## 开发环境 ##

- 操作系统： Windows7专业版
- Microsoft Visual Studio：Visual Studio 2010专业版

## 文件指引 ##

- src文件夹：eSDK\_IVS\_SDK\_Windows源码
- sample文件夹：eSDK IVS SDK的代码样例
- doc：eSDK IVS SDK的接口参考、开发指南


## 入门指导 ##

- 编译eSDK VCN for Windows：找到src\source\IVS\_SDK\_WINDOWS\IVS\_SDK\batch-compile.bat，已管理员身份运行打包脚本，编译并打包。脚本执行结束后，API包会生成在src\build\yyyymmdd\Windows\SDK，OCX包生成在src\build\yyyymmdd\Windows\OCX。（其中yyyymmdd为系统时间年月日）

- 编译运行API sample：打包脚本执行结束后，sample会生成在src\build\yyyymmdd\Windows\Demo\。解压eSDK_IVS_API_Demo_V2.1.00.100_CPP.zip并打开IVS_Demo.sln，选用debug或release环境下编译源代码。成功后会在.\Debug或者.\Release目录生成相应的可执行文件，用户可以双击运行。

- 编译运行OCX sample:
	1. 注册ocx：解压src\build\yyyymmdd\Windows\OCX\eSDK_IVS_OCX_V2.1.00.100_Windows.zip，再解压IVS_OCX.cab，已管理员身份运行regsvrOCX.bat。成功之后，会显示IVS_OCXPlayer.ocx注册成功。
	2. 解压src\build\yyyymmdd\Windows\Demo\eSDK_IVS_OCX_Demo_V2.1.00.100_CSharp.zip并打开ivs_cs_demo.sln，选用debug或release环境下编译源代码。成功后会在.\ivs_cs_demo\bin目录生成相应的可执行文件，用户可以双击运行。

- 详细的开发指南请参考doc中的开发指南

###登录VCN平台###
要体验华为VCN系统的能力，首先要登录VCN系统,以下代码演示如何登录VCN系统

    IVS_INT32 iRet = IVS_SDK_Init(0);//init IVS SDK

    // 登录信息
	IVS_LOGIN_INFO LoginReqInfo = {0};
	LoginReqInfo.stIP.uiIPType = IP_V4;
	// VCN平台IP
	strncpy_s(LoginReqInfo.stIP.cIP, “10.170.97.156”, IVS_IP_LEN);
	// VCN平台端口
	LoginReqInfo.uiPort = 9900;
	// 用户名，明文
	strncpy_s(LoginReqInfo.cUserName, “admin”, IVS_NAME_LEN);
	// 密码，明文
	strncpy_s(LoginReqInfo.pPWD, “7ui8&UI*”, IVS_PWD_LEN);
	// SessionID，用来标识一个用户的登录
	IVS_INT32 iSessionID = -1;
	// 登录
	iRet = IVS_SDK_Login(&LoginReqInfo, &iSessionID);

## 获取帮助 ##

在开发过程中，您有任何问题均可以至[DevCenter](https://devcenter.huawei.com)中提单跟踪。也可以在[华为开发者社区](http://bbs.csdn.net/forums/hwucdeveloper)中查找或提问。另外，华为技术支持热线电话：400-822-9999（转二次开发）