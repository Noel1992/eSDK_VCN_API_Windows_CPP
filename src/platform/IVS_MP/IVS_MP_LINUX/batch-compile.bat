::关闭回显
@echo off

::获得当前时间，作为生成版本的目录名
for /F "tokens=1-4 delims=-/ " %%i in ('date /t') do (
   set Year=%%i
   set Month=%%j
   set Day=%%k
   set DayOfWeek=%%l
)
for /F "tokens=1-2 delims=: " %%i in ('time /t') do (
   set Hour=%%i
   set Minute=%%j
)

::设置各变量名
set   	DateTime=%Year%-%Month%-%Day%-%Hour%-%Minute%
set	WinRarRoot=C:\Program Files\WinRAR
set	ProjectPath=%cd%\

@echo on
echo %DateTime%
@echo .
@echo －－－－－－－－－－－－－－开始编译Debug解决方案－－－－－－－－－－－－－－－
@echo .
::重编解决方案
"%VS100COMNTOOLS%\..\IDE\devenv.exe" "%ProjectPath%\IVS_Player.sln" /Rebuild "Debug|Win32" /out

::再编译一次，保证有依赖关系的项目能链接过
::"%VS100COMNTOOLS%\..\IDE\devenv.exe" "%ProjectPath%\IVS_Player.sln" /build "Debug|Win32" /out
@echo .
@echo －－－－－－－－－－－－－－开始编译Release解决方案－－－－－－－－－－－－－－－
@echo .
"%VS100COMNTOOLS%\..\IDE\devenv.exe" "%ProjectPath%\IVS_Player.sln" /Rebuild "Release|Win32" /out

::再编译一次，保证有依赖关系的项目能链接过
"%VS100COMNTOOLS%\..\IDE\devenv.exe" "%ProjectPath%\IVS_Player.sln" /build "Release|Win32" /out
@echo .
@echo －－－－－－－－－－－－－－解决方案编译完成－－－－－－－－－－－－－－－－	
@echo .
@echo －－－－－－－－－－－－－－开始打包版本－－－－－－－－－－－－－－－－	
@echo .
mkdir tempversion
@echo .
@echo －－－－－－－－－－－－－－开始拷贝版本－－－－－－－－－－－－－－－－	
@echo .
xcopy /y /i /r /s "inc\NetSourceCommon.h"                   				"tempversion\inc\"
xcopy /y /i /r /s "inc\IVS_NetSource.h"                   					"tempversion\inc\"
xcopy /y /i /r /s "inc\IVSPlayerDataType.h"                   				"tempversion\inc\"
xcopy /y /i /r /s "inc\IVS_Player.h"                   						"tempversion\inc\"

xcopy /y /i /r /s "build\windows\debug\IVS_NetSource\IVS_NetSourced.lib"	"tempversion\debug\lib\"
xcopy /y /i /r /s "build\windows\debug\IVS_NetSource\IVS_NetSourced.pdb"	"tempversion\debug\pdb\"
xcopy /y /i /r /s "build\windows\debug\IVS_NetSource\IVS_NetSourced.dll"	"tempversion\debug\bin\"
xcopy /y /i /r /s "build\windows\debug\IVS_Player\IVS_Playerd.lib"			"tempversion\debug\lib\"
xcopy /y /i /r /s "build\windows\debug\IVS_Player\IVS_Playerd.pdb"			"tempversion\debug\pdb\"
xcopy /y /i /r /s "build\windows\debug\IVS_Player\IVS_Playerd.dll"			"tempversion\debug\bin\"
xcopy /y /i /r /s "build\windows\debug\IVS_Player\mp_log.conf"					"tempversion\debug\bin\"
xcopy /y /i /r /s "lib\windows\Debug\mp4DLL.lib"							"tempversion\debug\lib\"
xcopy /y /i /r /s "lib\windows\Debug\mp4DLL.pdb"							"tempversion\debug\pdb\"
xcopy /y /i /r /s "lib\windows\Debug\mp4DLL.dll"							"tempversion\debug\bin\"
xcopy /y /i /r /s "lib\windows\Debug\cudart32_42_9.dll"						"tempversion\debug\bin\"
xcopy /y /i /r /s "lib\windows\Debug\libmpg123.dll"					    	"tempversion\debug\bin\"

xcopy /y /i /r /s "build\windows\release\IVS_NetSource\IVS_NetSource.dll"	"tempversion\release\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_NetSource\IVS_NetSource.pdb"	"tempversion\release\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_NetSource\IVS_NetSource.lib"	"tempversion\release\lib\"
xcopy /y /i /r /s "build\windows\release\IVS_Player\IVS_Player.dll"			"tempversion\release\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_Player\IVS_Player.pdb"			"tempversion\release\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_Player\IVS_Player.lib"			"tempversion\release\lib\"
xcopy /y /i /r /s "build\windows\debug\IVS_Player\mp_log.conf"				    "tempversion\release\bin\"
xcopy /y /i /r /s "lib\windows\Release\mp4DLL.lib"							"tempversion\release\lib\"
xcopy /y /i /r /s "lib\windows\Release\mp4DLL.pdb"							"tempversion\release\pdb\"
xcopy /y /i /r /s "lib\windows\Release\mp4DLL.dll"							"tempversion\release\bin\"
xcopy /y /i /r /s "lib\windows\Release\cudart32_42_9.dll"					"tempversion\release\bin\"
xcopy /y /i /r /s "lib\windows\Release\libmpg123.dll"					    "tempversion\release\bin\"


@echo .

cd tempversion
"%WinRarRoot%\WinRAR.exe" a -r MP_%DateTime%.rar .\
move MP_%DateTime%.rar ..\
cd ..
rd /Q /S tempversion

mkdir version
move MP_%DateTime%.rar .\version\
@echo －－－－－－－－－－－－－－打包版本完成－－－－－－－－－－－－－－－－	
@echo off

