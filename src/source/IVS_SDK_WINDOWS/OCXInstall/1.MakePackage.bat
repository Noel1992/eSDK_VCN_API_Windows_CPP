::����OCX�ļ��������Ŀ¼

::���ɰ�װ��
%INSTALLSHIELD% -p "%cd%\eSDK_IVS_Player.ism"

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
set   	DateTime=%Year%-%Month%-%Day%-%Hour%-%Minute%
set     DateOnly=%Year%-%Month%-%Day%
::������װ������ʱĿ¼
xcopy /y "%cd%\eSDK_IVS_Player\PROJECT_ASSISTANT\SINGLE_EXE_IMAGE\DiskImages\DISK1\eSDK_IVS_OCX.exe" "%cd%\"
del /q "%cd%\eSDK_IVS_Player\PROJECT_ASSISTANT\SINGLE_EXE_IMAGE\DiskImages\DISK1\eSDK_IVS_OCX.exe" 
::����ѹ����
"c:\Program Files\WinRAR\Rar.exe" a -r -ep1 "%cd%\eSDK_IVS_OCX_V1.5.00_%DateTime%.zip" "%cd%\eSDK_IVS_OCX.exe" 
::ɾ��exe�ļ�
del /q eSDK_IVS_OCX.exe


::����ѹ����������Ŀ¼
xcopy /y "%cd%\eSDK_IVS_OCX_V1.5.00_%DateTime%.zip" "..\..\..\..\00.Release\%DateOnly%\"