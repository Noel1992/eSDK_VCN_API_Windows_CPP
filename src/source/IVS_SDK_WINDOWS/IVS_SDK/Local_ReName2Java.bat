::�رջ���
@echo off
::���ø�������
set	ProjectPath=%cd%
@echo on
@echo .
@echo ����������������������������ΪJava������������������������������������������
call ThirdParty\Rename2Java.exe "%ProjectPath%\inc\IVS_SDK.h" "%ProjectPath%\IVS_SDK\Rename2Java_SDK.h"
call ThirdParty\Rename2Java.exe "%ProjectPath%\inc\IVS_SDKTVWall.h" "%ProjectPath%\IVS_SDK\Rename2Java_TVWall.h"
@echo .
@echo off
