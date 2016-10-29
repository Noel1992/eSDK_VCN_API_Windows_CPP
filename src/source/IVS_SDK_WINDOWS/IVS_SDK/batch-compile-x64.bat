::�رջ���
@echo off


::�汾��
::���汾���� ���޸Ĵ˴��汾��
set DistrVersion=1.5.70


::��õ�ǰʱ�䣬��Ϊ���ɰ汾��Ŀ¼��
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

set SDKTarget=eSDK_IVS_API_V%DistrVersion%_Windows.zip
set OCXTarget=eSDK_IVS_OCX_V%DistrVersion%_Windows.zip

set APIDemoCPPTarget=eSDK_IVS_API_Demo_V%DistrVersion%_CPP.zip
set APIDemoCSharpTarget=eSDK_IVS_API_Demo_V%DistrVersion%_CSharp.zip

set OCXDemoJSTarget=eSDK_IVS_OCX_Demo_V%DistrVersion%_JS.zip
set OCXDemoCSharpTarget=eSDK_IVS_OCX_Demo_V%DistrVersion%_CSharp.zip

set APIDemoRealPlayTarget=eSDK_IVS_API_Demo_V%DistrVersion%_Realplay_CPP.zip
set APIDemoPlaybackTarget=eSDK_IVS_API_Demo_V%DistrVersion%_Playback_CPP.zip


::���ø�������
set DateTime=%Year%-%Month%-%Day%-%Hour%-%Minute%
set	WinRarRoot=C:\Program Files\WinRAR
set	ProjectPath=%cd%

::���������ߵ�·��
set	ToolsPath=..\..\..\tools
::����demo��·��
set	TestDemoPath=..\..\..\test\demo

@echo on
echo %DateTime%

@echo .
@echo ����������������������������ɾ��ThirdParty�µ�MicroSoft��������������������������������
del /Q "%ProjectPath%\ThirdParty\cabarc.Exe"
del /Q "%ProjectPath%\ThirdParty\basicfiles\IEShims.dll"
del /Q "%ProjectPath%\ThirdParty\basicfiles\msvcp100.dll"
del /Q "%ProjectPath%\ThirdParty\basicfiles\msvcr100.dll"
del /Q "%ProjectPath%\ThirdParty\basicfiles\wer.dll"

@echo .
@echo ������������������������������ʼ����MicroSoft��ThirdParty������������������������������
xcopy /y /i /r /s "%ToolsPath%\MicroSoft\cabarc.Exe"                 "ThirdParty\"
xcopy /y /i /r /s "%ToolsPath%\MicroSoft\basicfiles\IEShims.dll"     "ThirdParty\basicfiles\"
xcopy /y /i /r /s "%ToolsPath%\MicroSoft\basicfiles\msvcp100.dll"    "ThirdParty\basicfiles\"
xcopy /y /i /r /s "%ToolsPath%\MicroSoft\basicfiles\msvcr100.dll"    "ThirdParty\basicfiles\"
xcopy /y /i /r /s "%ToolsPath%\MicroSoft\basicfiles\wer.dll"         "ThirdParty\basicfiles\"

@echo .
@echo ������������������������������ʼ����Release�������������������������������������
@echo .
"%VS100COMNTOOLS%\..\IDE\devenv.exe" "%ProjectPath%\IVS_SDK.sln" /rebuild "Release|x64" /out output.txt

@echo .
@echo �����������������������������������������ɣ�������������������������������	
@echo .
@echo ������������������������������ʼ����汾��������������������������������	
@echo .
mkdir tempversion
@echo .
@echo ������������������������������ʼ�����汾��������������������������������	
@echo .
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\ACE.dll"           		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\bp_base.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\bp_frame.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\bp_log.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\bp_net.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\ivs_base.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\ivs_compression.dll"	    "tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_IADraw.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_SDKTVWall.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_SDKNetKeyBoard.dll"	"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\ivs_security.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\ivs_sftp.dll"				"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\libeay32.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\libssh2.dll"				"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\nss.dll"	        		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\rtsp_client.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\ssleay32.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\zlibwapi.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_SDK.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_OCXPlayer.ocx"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_NetSource.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_Player.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\mp4DLL.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\regsvrOCX.bat"    			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\mp_log.conf"				"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\libmpg123.dll"	    		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\libmp3lame.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\lame_enc.dll"				"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\eSDKClientLogCfg.ini"		"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\eSDKLogAPI.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\log4cpp.dll"				"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\eSDK_Secure_C.dll"			"tempversion\bin\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\libsecurec.dll"			"tempversion\bin\"

@echo .
@echo ������������������������������ʼ����Windows�����⣭������������������������������	
@echo .
xcopy /y /i /r /s "ThirdParty\basicfiles\msvcp100.dll"		"tempversion\bin\"
xcopy /y /i /r /s "ThirdParty\basicfiles\msvcr100.dll"		"tempversion\bin\"

@echo .
@echo ������������������������������ʼ����Lib�ļ���������������������������������	
@echo .
xcopy /E /y /i /r /s "build\windows\release\IVS_SDK\bin\lib\protocols"     "tempversion\bin\lib\protocols"
xcopy /E /y /i /r /s "build\windows\release\IVS_SDK\bin\lib\services"	   "tempversion\bin\lib\services"

xcopy /E /y /i /r /s "build\windows\release\IVS_SDK\bin\Skin"              "tempversion\bin\Skin"
xcopy /E /y /i /r /s "build\windows\release\IVS_SDK\bin\i18n"              "tempversion\bin\i18n"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\defaultConfig\LocalConfig.xml"              "tempversion\bin\defaultConfig\"

@echo .
@echo ������������������������������ʼ���������ļ���������������������������������	
@echo .
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\config\log.xml"        "tempversion\bin\config\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\config\net.xml"	    "tempversion\bin\config\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\config\service.xml"	"tempversion\bin\config\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\config\SDKConfig.xml"  "tempversion\bin\config\"

@echo .
@echo ������������������������������ʼ����SDK ͷ�ļ���������������������������������	
@echo .
::����SDKͷ�ļ���SDK���Ŀ¼
xcopy /y /i /r /s "build\windows\include\IVS_SDK.h"                   	"tempversion\include\"
xcopy /y /i /r /s "inc\IVS_SDKTVWall.h"                 				"tempversion\include\"
xcopy /y /i /r /s "..\..\..\platform\IVS_COMMON\inc\hwsdk.h"	    	"tempversion\include\"
xcopy /y /i /r /s "..\..\..\platform\IVS_COMMON\inc\ivs_error.h"   		"tempversion\include\"

::ɾ��demoĿ¼�µ�SDKͷ�ļ�
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_API_Demo\include\*"
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_PlatPlayerBack_Demo\include\*"
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_RealPlayer_Demo\include\*"

::����SDKͷ�ļ�������demoĿ¼
xcopy /y /i /r /e "tempversion\include\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_API_Demo\include\"
xcopy /y /i /r /e "tempversion\include\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_PlatPlayerBack_Demo\include\"
xcopy /y /i /r /e "tempversion\include\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_RealPlayer_Demo\include\"


@echo .
@echo ������������������������������ʼ����SDK lib�ļ���������������������������������	
@echo .
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_SDK.lib"       "tempversion\sdklib\"

::ɾ��demoĿ¼��SDK lib�ļ�
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_API_Demo\sdklib\*"
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_PlatPlayerBack_Demo\sdklib\*"
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_RealPlayer_Demo\sdklib\*"

::����SSDK lib�ļ�������demoĿ¼
xcopy /y /i /r /e "tempversion\sdklib\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_API_Demo\sdklib\"
xcopy /y /i /r /e "tempversion\sdklib\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_PlatPlayerBack_Demo\sdklib\"
xcopy /y /i /r /e "tempversion\sdklib\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_RealPlayer_Demo\sdklib\"

@echo .
@echo ������������������������������ʼ����pdb��������������������������������	
@echo .
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_NetSource.pdb"	"tempversion\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_OCXPlayer.pdb"	"tempversion\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_Player.pdb"	"tempversion\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_SDK.pdb"	    "tempversion\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\rtsp_client.pdb"	"tempversion\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\mp4DLL.pdb"	    "tempversion\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\IVS_IADraw.pdb"	"tempversion\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\eSDKLogAPI.pdb"	"tempversion\pdb\"
xcopy /y /i /r /s "build\windows\release\IVS_SDK\bin\eSDK_Secure_C.pdb"	"tempversion\pdb\"

@echo .
@echo ������������������������������ɾ��demo����Ŀ¼��������������������������������	
@echo .
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_API_Demo\Debug\*"
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_PlatPlayerBack_Demo\Debug\*"
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_RealPlayer_Demo\Debug\*"

del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_API_Demo\Release\*"
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_PlatPlayerBack_Demo\Release\*"
del /Q "%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_RealPlayer_Demo\Release\*"

del /Q "%TestDemoPath%\ivs_cs_demo_sdk\CSharp\IVS_SDK_Demo\IVS_SDK_Demo\bin\*"

del /Q "%TestDemoPath%\ivs_bs_demo\WebContent\ocx\*"

@echo .
@echo ������������������������������ʼ����binĿ¼������demo����Ŀ¼��������������������������������	
@echo .
xcopy /y /i /r /e "tempversion\bin\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_API_Demo\Debug\"
xcopy /y /i /r /e "tempversion\bin\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_PlatPlayerBack_Demo\Debug\"
xcopy /y /i /r /e "tempversion\bin\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_RealPlayer_Demo\Debug\"

xcopy /y /i /r /e "tempversion\bin\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_API_Demo\Release\"
xcopy /y /i /r /e "tempversion\bin\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_PlatPlayerBack_Demo\Release\"
xcopy /y /i /r /e "tempversion\bin\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\C++\IVS_RealPlayer_Demo\Release\"

xcopy /y /i /r /e "tempversion\bin\*"                   	"%TestDemoPath%\ivs_cs_demo_sdk\CSharp\IVS_SDK_Demo\IVS_SDK_Demo\bin\"

::������־�Ѽ������ļ���readme.txt�ļ�
xcopy /y /i /r "..\..\..\platform\eSDKClientLogAPI\*.txt"	        	"tempversion\"
xcopy /y /i /r "..\readme.txt"	        	"tempversion\"

cd tempversion
"%WinRarRoot%\WinRAR.exe" a -r %SDKTarget% .\
move %SDKTarget% ..\
cd ..
rd /Q /S tempversion

mkdir version
move %SDKTarget% .\version\

@echo ������������������������������ʼ���CAB�汾��������������������������������	
@echo .

mkdir tempcabversion\IVS_SDK
@echo .
"%WinRarRoot%\WinRAR.exe" x -y "version\%SDKTarget%"  tempcabversion\IVS_SDK
::ɾ��Windows API����־�ϴ�ͳ��ģ��������ļ�
del /Q "tempcabversion\IVS_SDK\bin\eSDKClientLogCfg.ini"
::����OCX����־�ϴ�ͳ��ģ�������ļ�
xcopy /E /y /i /r /s "..\..\..\platform\eSDKClientLogAPI\windows\WINOCXCfg\eSDKClientLogCfg.ini"	        	"tempcabversion\IVS_SDK\bin\"

@echo ��������������������������������web�����ļ���������������������������������
xcopy /y /i /r /s "ThirdParty\basicfiles\IEShims.dll"	                    "tempcabversion\IVS_SDK\bin\"
xcopy /y /i /r /s "ThirdParty\basicfiles\IVS_OCXPlayer.inf"	            	"tempcabversion\IVS_SDK\bin\"
xcopy /y /i /r /s "ThirdParty\basicfiles\License_Information.docx"	        "tempcabversion\IVS_SDK\bin\"
xcopy /y /i /r /s "ThirdParty\basicfiles\Microsoft.VC90.CRT.manifest"	    "tempcabversion\IVS_SDK\bin\"
xcopy /y /i /r /s "ThirdParty\basicfiles\msvcp100.dll"	                    "tempcabversion\IVS_SDK\bin\"
xcopy /y /i /r /s "ThirdParty\basicfiles\msvcr100.dll"	                    "tempcabversion\IVS_SDK\bin\"
xcopy /y /i /r /s "ThirdParty\basicfiles\wer.dll"	                        "tempcabversion\IVS_SDK\bin\"

mkdir versioncab
cd tempcabversion\IVS_SDK\bin
%ProjectPath%\ThirdParty\cabarc -r -p N %ProjectPath%\versioncab\IVS_OCX.cab *

cd %ProjectPath%\versioncab\

::ɾ��bs demo�µ�cab�ļ�
del /Q "%TestDemoPath%\ivs_bs_demo\WebContent\ocx\*"
::����cab�ļ���bs demo��
xcopy /y /i /r /s "IVS_OCX.cab"	                      "%TestDemoPath%\ivs_bs_demo\WebContent\ocx\"

::�������cab�ļ�
"%WinRarRoot%\WinRAR.exe" a -r %OCXTarget% .\
del /Q .\IVS_OCX.cab

cd %ProjectPath%
rd /Q /S tempcabversion

@echo ������������������������������ʼ���Demo��������������������������������	
@echo .
cd %TestDemoPath%
mkdir versioncdemo

cd .\ivs_cs_demo_sdk\C++\IVS_API_Demo
"%WinRarRoot%\WinRAR.exe" a -r %APIDemoCPPTarget% .\
move %APIDemoCPPTarget% ..\..\..\versioncdemo\

cd ..\IVS_RealPlayer_Demo
"%WinRarRoot%\WinRAR.exe" a -r %APIDemoRealPlayTarget% .\
move %APIDemoRealPlayTarget% ..\..\..\versioncdemo\

cd ..\IVS_PlatPlayerBack_Demo
"%WinRarRoot%\WinRAR.exe" a -r %APIDemoPlaybackTarget% .\
move %APIDemoPlaybackTarget% ..\..\..\versioncdemo\

cd ..\..\CSharp\IVS_SDK_Demo
"%WinRarRoot%\WinRAR.exe" a -r %APIDemoCSharpTarget% .\
move %APIDemoCSharpTarget% ..\..\..\versioncdemo\

cd ..\..\..\ivs_bs_demo
"%WinRarRoot%\WinRAR.exe" a -r %OCXDemoJSTarget% .\
move %OCXDemoJSTarget% ..\versioncdemo\

cd ..\ivs_cs_demo_ocx\eSDK_IVS_OCX_Demo_CSharp
"%WinRarRoot%\WinRAR.exe" a -r %OCXDemoCSharpTarget% .\
move %OCXDemoCSharpTarget% ..\versioncdemo\

cd %ProjectPath%

@echo ��������������������������������汾��ɣ�������������������������������	
@echo off

@echo ��������������������������������������Ŀ¼��������������������������������
::������ڵ���3�㣬
if "%Hour%" geq "03" goto CheckifXiaoyudengyu7dian
goto CopytoDebugDir
::�ж��Ƿ�С�ڵ���7�㣬�����
:CheckifXiaoyudengyu7dian
if "%Hour%" leq "07" goto CopytoReleaseDir
goto CopytoDebugDir
:CopytoReleaseDir
::���ڵ���3��С�ڵ���7��ʱ����ڵİ��ŵ�����������+ת�⡱Ŀ¼��
xcopy /y /i /r /s  "%ProjectPath%\version\%SDKTarget%"  "%ProjectPath%\..\..\..\build\%Year%%Month%%Day%ת��\Windows\SDK\"
xcopy /y /i /r /s  "%ProjectPath%\versioncab\%OCXTarget%"  "%ProjectPath%\..\..\..\build\%Year%%Month%%Day%ת��\Windows\OCX\"
xcopy /y /i /r /s  "%TestDemoPath%\versioncdemo\*"  "%ProjectPath%\..\..\..\build\%Year%%Month%%Day%ת��\Windows\Demo\"
::���Ƶ��Զ�������ָ����Ŀ¼
copy /y "%ProjectPath%\version\%SDKTarget%"  "D:\ftpServer\IVS_Client\IVS_SDK.rar"
goto End
::С��3������7��ʱ����ڵİ��ŵ����������ڡ�Ŀ¼��
:CopytoDebugDir
xcopy /y /i /r /s  "%ProjectPath%\version\%SDKTarget%"  "%ProjectPath%\..\..\..\build\%Year%%Month%%Day%\Windows\SDK\"
xcopy /y /i /r /s  "%ProjectPath%\versioncab\%OCXTarget%"  "%ProjectPath%\..\..\..\build\%Year%%Month%%Day%\Windows\OCX\"
xcopy /y /i /r /s  "%TestDemoPath%\versioncdemo\*"  "%ProjectPath%\..\..\..\build\%Year%%Month%%Day%\Windows\Demo\"
:End
@echo off