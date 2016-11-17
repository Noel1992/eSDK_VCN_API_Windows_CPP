#ifndef _DEMO_DEFINE_H_
#define _DEMO_DEFINE_H_

#define SDK_SESSIONID_INIT			(-1)//SessionID初始值
#define WM_UPDATEDEVICELIST		(WM_APP + 1000)
#define WM_UPDATEPRESETLIST		(WM_APP + 1001)
#define WM_UPDATERECORDLIST		(WM_APP + 1002)
#define WM_DOWNLOADSUCCESS	(WM_APP + 1003)
#define WM_UPDATEALARMLIST		(WM_APP + 1004)
#define WM_UPDATEBRODCASTLIST		(WM_APP + 1005)

// 判断指针是否为空，无返回值 
#define CHECK_POINTER_VOID(pPointer)	\
	if (NULL == (pPointer))	\
	{									\
		return;						\
	}									\

// 判断指针是否为空，有返回值
#define CHECK_POINTER(pPointer, retValue)	\
	if (NULL == (pPointer))	\
	{									\
		return (retValue);					\
	}									\

//视屏播放类型
typedef enum
{
	NOT_SET,							//未定
	REAL_PLAY,						//实况
	LOCAL_PLAY_BACK,			//本地回放
	PLATFORM_PLAY_BACK,	//平台回放
	PU_PLAY_BACK 				//前端回放
}IVS_PLAY_TYPE;

#endif
