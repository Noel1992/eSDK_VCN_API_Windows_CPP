::拷贝OCX文件到待打包目录

::生成安装包
%INSTALLSHIELD% -p "%cd%\eSDK_IVS_Player.ism"

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
set   	DateTime=%Year%-%Month%-%Day%-%Hour%-%Minute%
set     DateOnly=%Year%-%Month%-%Day%
::拷贝安装包到临时目录
xcopy /y "%cd%\eSDK_IVS_Player\PROJECT_ASSISTANT\SINGLE_EXE_IMAGE\DiskImages\DISK1\eSDK_IVS_OCX.exe" "%cd%\"
del /q "%cd%\eSDK_IVS_Player\PROJECT_ASSISTANT\SINGLE_EXE_IMAGE\DiskImages\DISK1\eSDK_IVS_OCX.exe" 
::生成压缩包
"c:\Program Files\WinRAR\Rar.exe" a -r -ep1 "%cd%\eSDK_IVS_OCX_V1.5.00_%DateTime%.zip" "%cd%\eSDK_IVS_OCX.exe" 
::删除exe文件
del /q eSDK_IVS_OCX.exe


::拷贝压缩包到发布目录
xcopy /y "%cd%\eSDK_IVS_OCX_V1.5.00_%DateTime%.zip" "..\..\..\..\00.Release\%DateOnly%\"