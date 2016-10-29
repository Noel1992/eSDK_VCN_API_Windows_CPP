#ifndef MP4_LIB_INTERFACE
#define MP4_LIB_INTERFACE

#define MAX_INSTANCE_NUM 128
//extern const int MAX_INSTANCE_NUM;
#define INSTANCE_ERROR 0xffffffff	//�������,�ӿڵ���˳���������������޵����⣬���˴���
#define INSTANCE_OK	   0x00000000
//begin add by zhaobo 2011-02-10  ,���������
#define DISKFULL_ERROR  0xfffffffe	//д���ݴ���,���̿ռ䲻�㣬���˴���
#define MEMORY_ERROR	0xfffffffd	//�ڴ�����ڴ�����������ʱ�����˴���
#define PARA_ERROR		0xfffffffc	//������δ������Ϊ�ջ�������Ͳ�����Ҫ��ʱ�����˴���
#define FRAME_ERROR		0xfffffffb	//֡���󣬵�ĳһ֡�����쳣ʱ(֡�����̣��޷��ж�֡����)�����˴��󣬴˴���Ӱ�����֡��д�룬���Ժ��ӡ�
#define FILEFULL_ERROR	0xfffffffa	//�ļ���С��������
#define OTHER_ERROR		0xfffffff9	//��������
#define INSTANCE_FULL	0xfffffff8
//end of add by zhaobo 2011-2-10
#define CREAT_FILE_FAILED 0xfffffff8	//�����ļ�ʧ�ܣ�����û��Ȩ�ޣ�����·������
//add by chenyuanlong 2011_8_3
#define  RESOLUTION_CHANGED 0xfffffff4

#define H264	99
#define MPEG4   98  //����ϵ�
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