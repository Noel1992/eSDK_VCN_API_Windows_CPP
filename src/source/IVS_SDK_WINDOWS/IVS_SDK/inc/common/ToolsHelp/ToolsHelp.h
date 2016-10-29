/*Copyright 2015 Huawei Technologies Co., Ltd. All rights reserved.
eSDK is licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

/********************************************************************
	filename	: 	ToolsHelp.h
	author		:	s00191067
	created		:	2011/10/28	
	description	:	�ṩ����ģ�鹫�������Ĺ����ļ�
	copyright	:	Copyright (C) 2011-2015
	history		:	2011/10/28
*********************************************************************/

#ifndef __TOOLSHELP_H__
#define __TOOLSHELP_H__

/*!< Ԥ����ͷ�ļ� */

/*!< ����PCLint */
#include "ivs_xml.h"
#include "hwsdk.h"
#include "SDKDef.h"

#ifdef WIN32
#define sscanf eSDK_SSCANF //lint !e683
#endif

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

typedef std::map <std::string, std::string> HttpHeadInfoMap;

class CToolsHelp
{
public:

    /***********************************************************************************
    * name       : Int2Str
    * description: ������ת��Ϊ�ַ���
    * input      : iValue:Ҫת����������ֵ; bHex:�Ƿ���ʮ������������
    * output     : NA
    * return     : ת������ַ���
    * remark     : NA
    ***********************************************************************************/
    static std::string Int2Str ( int iValue, bool bHex = false );

	 /***********************************************************************************
    * name       : ItoaEX
    * description: ��ʮ��������ת��Ϊ�ַ���
    * input      : iValue:Ҫת����������ֵ;
    * output     : buff:ת������ַ���
    * return     : �ɹ���ת������ַ���ʵ���ַ�����;ʧ�ܷ���-1;
    * remark     : NA
    ***********************************************************************************/
    static int ItoaEX ( int iValue, char *buff );
	
    /***********************************************************************************
    * name       : StrToInt
    * description: ������ת��Ϊ�ַ���
    * input      : strSrc:Ҫת�����ַ���; bHex:�Ƿ���ʮ������������
    * output     : NA
    * return     : ת�����������
    * remark     : NA
    ***********************************************************************************/
	static int StrToInt( const std::string& strSrc, bool bHex = false );
	static int StrToInt( char* pSrc, bool bHex = false );

    /**************************************************************************
    * name       : GetSCCHead
    * description: ��¼SCC��������ͷ
    * input      : NA
    * output     : NA
    * return     : SCC������PORT
    * remark     : NA
    **************************************************************************/
	static std::string GetSCCHead();

	static int ResultCodeToErrorCode(int iResultCode);

    static bool IsNum(const std::string &str);

    /**************************************************************************
    * name       : GetHostName
    * description: ��ȡ�����������
    * input      : 
    * output     : pHostName �����������
    * return     : int �ɹ�����0��ʧ�ܷ���1
    * remark     : NA
    **************************************************************************/
    static int GetHostName(char* pHostName, IVS_UINT32 uiNameLen);

    /**************************************************************************
    * name       : GetCurrentUserName
    * description: ��ȡ�������ǰ��¼�û���
    * input      : 
    * output     : pUserName ��ǰ��¼�û���
    * return     : int �ɹ�����0��ʧ�ܷ���1
    * remark     : NA
    **************************************************************************/
	static int GetCurrentUserName(char* pUserName);

    /******************************************************************
     function   : get_exe_parent_dir
     description: ��ȡ��������Ŀ¼�ĸ�Ŀ¼
     output      : char * buf
     return     : IVS_INT32 �ɹ�����0�� ʧ�ܷ��ش�����
    *******************************************************************/
    static int get_exe_parent_dir(char *buf);

    /******************************************************************
     function   : get_home_directory
     description: ��ȡ��Ŀ¼
     output      : std::string & strDir
     return     : IVS_INT32
    *******************************************************************/
    static int get_home_directory(std::string& strDir);

    /******************************************************************
     function   : get_config_directory
     description: ��ȡconfigĿ¼
     output      : std::string & strDir
     return     : IVS_INT32
    *******************************************************************/
    static int get_config_directory(std::string& strDir);

	/**************************************************************************
	* name       : TranslateToSer
	* description: �����������֮������ݷ���
	* input      : iNum		���������
	*			   pscAgree	���϶��ʵ��ַ���
			       pscDisAgree ���񶨴ʵ��ַ���
	  output     : NA
	* return     : string
	* remark     : NA
    **************************************************************************/
	static const char*  TranslateToSer(const char *pscAgree, const char *pscDisAgree, int &iNum);

	/**************************************************************************
	* name       : TranslateToSer
	* description: �����������֮������ݷ���
	* input      : pscTemp		������Ĵ�
	  output     : iNum ��������
	* return     : �ɹ����ط�����������
	* remark     : NA
    **************************************************************************/
	static int TranslateFromSer(const char *pscTemp, int &iNum);

	    /***********************************************************************************
    * name       : LTrim
    * description: strSrcȥ����ո�
    * input      : src; 
    * output     : 
    * return     : ���ش�д��string
    * remark     : NA
    ***********************************************************************************/
    static void LTrim(std::string& strSrc);

    /***********************************************************************************
    * name       : LTrim
    * description: strSrcȥ���ҿո�
    * input      : src; 
    * output     : 
    * return     : ���ش�д��string
    * remark     : NA
    ***********************************************************************************/
    static void RTrim(std::string& strSrc);

    /***********************************************************************************
    * name       : LTrim
    * description: strSrcȥ�����ҿո�
    * input      : src; 
    * output     : 
    * return     : ���ش�д��string
    * remark     : NA
    ***********************************************************************************/
    static std::string Trim(std::string& strSrc);



	static wchar_t* ANSIToUnicode(const char* str);
	static wchar_t* UTF8ToUnicode(const char* str);
	static char* UnicodeToUTF8(const wchar_t* str);
	static char* UnicodeToANSI(const wchar_t* str);
	static char* UTF8ToANSI(const char* str);
	static char* ANSIToUTF8(const char* str);

    /******************************************************************
     function   : SplitCameraDomainCode
     description: ��CU��������cameraid�Ǻϲ����,Ҫ��ֳ�cameracode��domaincode��Ϊ������ǰ�İ汾��
                  ���strCameraDomainCode��û��#ƴװ�ֶΣ�ֱ�Ӹ�ֵ��strCameraCode
     input      : const std::string & strCameraDomainCode   CU�˵ĺϲ���Code����ʽΪ���������#�����
     input      : std::string & strCameraCode   ���������
     input      : std::string & strDomainCode   �����
     output     : NA
     return     : int
    *******************************************************************/
    static int SplitCameraDomainCode(const std::string& strCameraDomainCode, 
                                     std::string& strCameraCode, 
                                     std::string& strDomainCode);


    /******************************************************************
     function   : IsNeedReDirect
     description: �ж������Ƿ���Ҫ�ض���;�������������ص���������ض���;
     input      : unsigned short iReqID ����ID
     output     : NA
     return     : bool true��Ҫ�ض���; false����Ҫ;
    *******************************************************************/
    static bool IsNeedReDirect(unsigned short iReqID);

    /**************************************************************************
    * name       : FindElementByPath
    * description: ����·����ȡһ���ڵ�
    * input      : 
                   pParentEle TiXmlElement* ���ڵ�����
                   strPath string& �ڵ�·��
    * return     : TiXmlElement*  �������һ���ڵ��ָ��
    * remark     : NA
    **************************************************************************/
    static TiXmlElement* FindElementByPath(TiXmlNode* pParentEle,std::string strPath);

	/***********************************************************************************
    * name       : Long2Str
    * description: ��������ת��Ϊ�ַ���
    * input      : iValue:��ת��������; bHex:�Ƿ�Ϊ16����
    * output     : NA
    * return     : ת������ַ���
    * remark     : NA
    ***********************************************************************************/
    static std::string Long2Str ( long lValue, bool bHex = false );

	/***********************************************************************************
    * name       : ULong2Str
    * description: ���޷�������ת��Ϊ�ַ���
    * input      : iValue:��ת��������; bHex:�Ƿ�Ϊ16����
    * output     : NA
    * return     : ת������ַ���
    * remark     : NA
    ***********************************************************************************/
    static std::string ULong2Str (unsigned long lValue, bool bHex = false );

    /******************************************************************
     function   : IsSpeedVaild
     description: �ж��Ƿ�Ϊ�Ϸ��Ĳ��ű���;���������ı���Ϊ��-32 -16 -8 -4 -2 ��-1 1 2 4�� 8 16 32��
     input      : float fScale  ���ű���
     output     : NA
     return     : bool true �Ϸ���false���Ϸ�;
    *******************************************************************/
    static bool IsSpeedVaild(float fScale);

    /******************************************************************
     function   : GetSendScale
     description: ���û������ٶ�ת��Ϊ����������͵Ĳ����ٶȣ���Ӧ��ϵ���£�
                  �û������ٶ�                    �����������ٶ�
                  1                                     2       2����ȫ��
                  2                                     4       4����ȫ��
                  4                                     254     8����ȫ��ǰ��
                  8                                     16      16����ֻ��I֡
                  16                                    32      32����ֻ��I֡ 
                  -1                                    253     2����ȫ������
                  -2                                    -4      4����ֻ��I֡
                  -4                                    -8      8����ֻ��I֡
                  -8                                    -16     16����ֻ��I֡
                  -16                                   -32     //������ȷ��
                  0.5                                   1       1����ȫ��
                  0.25                                  0.5     1/2ȫ��
                  0.125     8��֮1��˳�򲥷�            0.25    1/4ȫ��
                  0.0625    16��֮1��˳�򲥷�           0.125   1/8ȫ��
                  0.03125   32��֮1��˳�򲥷�           0.0625  1/16ȫ��
                  -0.5      2��֮1�ٺ��˲���            252     1����ȫ������
                  -0.25     4��֮1�ٺ��˲���            252     1����ȫ������
     input      : IVS_SERVICE_TYPE enServiceType
	 input      : float fUserScale
     input      : float & fSendScale
     output     : NA
     return     : bool
    *******************************************************************/
    static bool GetSendScale(IVS_SERVICE_TYPE enServiceType, float fUserScale, float& fSendScale);

    /**************************************************************************
    * name       : MemCpy
    * description: ��װ���memcpy����
    * input      : pDst     ָ��Ŀ���ڴ�ָ�룻
                   uiMaxLen   Ŀ���ڴ���󳤶ȣ�
                   pSrc     ָ����ʼ�ڴ�ָ�룻
                   uiCopyLen  �������ȡ�
    * output     : NA
    * return     : �ɹ�����true��ʧ�ܷ���false
    * remark     : NA
	**************************************************************************/
	static bool Memcpy(void* pDst, unsigned int uiMaxLen, const void* pSrc, unsigned int uiCopyLen);

    /**************************************************************************
    * name       : Strncpy
    * description: ��װ���strncpy����,�����ɹ����Զ����ַ�����β���'\0��
    * input      : pDst     ָ��Ŀ���ַ�����ָ�룻
                   MaxLen   ������󳤶ȣ�
                   pSrc     ָ����ʼ�ַ�����ָ�룻
                   CopyLen  ��������,��������\0���ĳ��ȡ�
    * output     : NA
    * return     : �ɹ�����true��ʧ�ܷ���false
    * remark     : NA
    **************************************************************************/
    static bool Strncpy(char* pDst, unsigned int uiMaxLen, const char* pSrc, unsigned int uiCopyLen);

    /**************************************************************************
    * name       : Snprintf
    * description: ��װ���snprintf�����������ɹ����Զ����ַ�����β���'\0��
    * input      : pBuffer  ָ���ʽ���ַ�����洢�ڴ��ָ�룻
                   MaxLen   pBuffer������󳤶ȣ�
                   pFormat  ָ���ʽ�������б��ָ�롣
    * output     : NA
    * return     : �ɹ�����true��ʧ�ܷ���false
    * remark     : NA
    **************************************************************************/
    static bool Snprintf(char* pBuffer, unsigned int uiMaxLen, const char* pFormat, ...);

    /**************************************************************************
    * name       : Strncat
    * description: ��װ���strncat����
    * input      : pDst     ָ��Ŀ���ַ�����ָ�룻
                   uiMaxLen Ŀ���ַ�����󳤶ȣ�
                   pSrc     ָ����ʼ�ַ�����ָ�롣
    * output     : NA
    * return     : �ɹ�����true��ʧ�ܷ���false
    * remark     : NA
    **************************************************************************/
    static bool Strncat(char* pDst, unsigned int uiMaxLen,const char* pSrc);

    /******************************************************************
     function   : replace_all
     description: �滻����ָ�����ַ���
     input      : std::string & str
     input      : const std::string & old_value
     input      : const std::string & new_value
     output     : NA
     return     : std::string&
    *******************************************************************/
    static std::string& replace_all(std::string& str,const std::string& old_value,const std::string& new_value);

    /******************************************************************
     function   : replace_all_distinct
     description: �滻����ָ�����ַ���
     input      : std::string & str
     input      : const std::string & old_value
     input      : const std::string & new_value
     output     : NA
     return     : std::string&
    *******************************************************************/
    static std::string& replace_all_distinct(std::string& str,const std::string& old_value,const std::string& new_value);

	/******************************************************************
     function   : GetDLLPath
     description:��ȡDLL��ִ��·��	
     output      : std::string & strPath
     input      : const std::string & strDLLName
     return     : std::string&
    *******************************************************************/
    static void GetDLLPath(const std::string& strDLLName, std::string& strPath);

	/******************************************************************
     function   : UTCStr2Time
     description: ��UTCʱ�䴮��ʽ��YYYY,MM,DD,hh,mm,ss�ֶ�ת�����������Ϊ��λ������ʱ��,
                  ���Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00����������������
     input      : const char * pStr     ��ת�����ַ���
     input      : const char * pFormat  �ַ�����Ӧ�ĸ�ʽ;
     output     : NA
     return     : time_t ת�����ʱ��
    *******************************************************************/
    static time_t UTCStr2Time(const char *pStr, const char *pFormat);

    /******************************************************************
     function   : UTCTime2Str
     description: ��UTCʱ�䴮��ʽ��YYYY,MM,DD,hh,mm,ss�ֶ�ת�����������Ϊ��λ������ʱ��,
                  ���Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00����������������
     input      : const char * pStr     ��ת�����ַ���
     input      : const char * pFormat  �ַ�����Ӧ�ĸ�ʽ;
     output     : NA
     return     : time_t ת�����ʱ��
    *******************************************************************/
    static void UTCTime2Str(const time_t t, std::string& strTime, const std::string& strFormat);

	/******************************************************************
     function   : GetOcxPathFromCLSID
	 description: get OCX path from CLSID.
	 input      : classId, CLSID
	 output     : sPath
	 input      : nSize, size of path buffer
	 return     : success return 0, else return errno
    *******************************************************************/
	static IVS_INT32 GetOcxPathFromCLSID(const IVS_CHAR *classId, IVS_CHAR *sPath, IVS_UINT32 nSize);

private:
	/******************************************************************
     function   : Str2Time
     description: ��ʱ�䴮��ʽ��YYYY,MM,DD,hh,mm,ss�ֶ�ת�����������Ϊ��λ������ʱ��,
                  ���Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00����������������
     input      : const char * pStr
     input      : const char * pFormat
     output     : NA
     return     : time_t
    *******************************************************************/
    static time_t Str2Time(const char *pStr, const char *pFormat);

    /******************************************************************
     function   : Time2Str
	 description: ������ʱ�䣨�Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00����������������
	 *              ת��Ϊ����"20090630091030"���ַ���
	 input      : char *pDestBuf������ʱ���ַ���������ָ�루ֵ�����������int nbuflen������ʱ���ַ�����������С��
	              const time_t *calptr���Թ��ʱ�׼ʱ�乫Ԫ1970��1��1��00:00:00��������������
     output     : ����ָ��pDestBuf
     return     : time_t
    *******************************************************************/
    static void Time2Str(const time_t t, std::string& strTime, const std::string& strFormat);

	

};

#endif //__TOOLSHELP_H__               
