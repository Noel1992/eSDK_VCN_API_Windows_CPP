::关闭回显
@echo off
::设置各变量名
set	ProjectPath=%cd%
@echo on
@echo .
@echo －－－－－－－－－－－－－－为Java重命名函数名－－－－－－－－－－－－－－－
call ThirdParty\Rename2Java.exe "%ProjectPath%\inc\IVS_SDK.h" "%ProjectPath%\IVS_SDK\Rename2Java_SDK.h"
call ThirdParty\Rename2Java.exe "%ProjectPath%\inc\IVS_SDKTVWall.h" "%ProjectPath%\IVS_SDK\Rename2Java_TVWall.h"
@echo .
@echo off
