#ifndef MP4_LIB_INTERFACE
#define MP4_LIB_INTERFACE

#define MAX_INSTANCE_NUM 128
//extern const int MAX_INSTANCE_NUM;
#define INSTANCE_ERROR 0xffffffff	//句柄错误,接口调用顺序出错，句柄超过上限等问题，报此错误
#define INSTANCE_OK	   0x00000000
//begin add by zhaobo 2011-02-10  ,错误码添加
#define DISKFULL_ERROR  0xfffffffe	//写数据错误,磁盘空间不足，报此错误
#define MEMORY_ERROR	0xfffffffd	//内存错误，内存分配出现问题时，报此错误
#define PARA_ERROR		0xfffffffc	//函数入参错误，入参为空或入参类型不符合要求时，报此错误
#define FRAME_ERROR		0xfffffffb	//帧错误，当某一帧出现异常时(帧长过短，无法判断帧类型)，报此错误，此错误不影响后续帧的写入，可以忽视。
#define FILEFULL_ERROR	0xfffffffa	//文件大小超过限制
#define OTHER_ERROR		0xfffffff9	//其他错误
#define INSTANCE_FULL	0xfffffff8
//end of add by zhaobo 2011-2-10
#define CREAT_FILE_FAILED 0xfffffff8	//创建文件失败，例如没有权限，或者路径错误
//add by chenyuanlong 2011_8_3
#define  RESOLUTION_CHANGED 0xfffffff4

#define H264	99
#define MPEG4   98  //后加上的
#define MuLow	0
#define aLow	8

#ifdef __cplusplus
extern "C" {
#endif 

long __stdcall Init();
long __stdcall GetInstance(long *lHandle, long lvideotype, long laudiotype);		//#0 >= #1


long __stdcall StartWrite(long lHandle, char* pszFileName);				//#1 => #2
long __stdcall SetVideoFrameRate(long lHandle, long lFrameRate);			//#2
long __stdcall InsertVideoBuffer(long lHandle ,void* pBuf,unsigned long lBufLen, int type);	//#2
long __stdcall SetAudioParameter(long lHandle, long SampleRate,long ChannelNumbers,long RawBits);//#2
long __stdcall InsertAudioBuffer(long lHandle ,void* pBuf,unsigned long lBufLen, int type);	//#2
long __stdcall Stop(long lHandle);		//=>#1

long __stdcall Release();
long __stdcall ReleaseInstance(long lHandle);		//#13 => #0

#ifdef __cplusplus
}
#endif

#endif