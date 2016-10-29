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


/******************************************************************************
   ��Ȩ���� (C), 2008-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �ļ���          : CRingCache.cpp
  �汾��          : 1.0
  ����            : xuxin 00114494
  ��������        : 2008-08-07
  ����޸�        : 
  ��������        : ���λ�����
  �����б�        : 
  �޸���ʷ        : 
  1 ����          : 
    ����          : 
    �޸�����      : 
*******************************************************************************/


#include "RingCache.h"
#include "eSDK_Securec.h"

//�������жദ������֤�����⣬ע���㷨Ч�ʣ�������ȥ����PC��LINT
/*lint -e414*/
/*lint -e429*/
/*lint -e613*/
/*lint -e662*/
/*lint -e668*/
/*lint -e669*/
/*lint -e670*/
/*lint -e826*/
/*lint -e1714*/


CRingCache::CRingCache()
{
    m_pBuffer = NULL;
    m_ulBufferSize = 0;
    m_ulReader = 0;
    m_ulWriter = 0;
    m_ulDataSize = 0;

    ::InitializeCriticalSectionAndSpinCount(&m_cs, 4000 ); 
}

CRingCache::~CRingCache()
{  
    ::EnterCriticalSection(&m_cs);
    if(NULL != m_pBuffer)
    {
        __try
        {
            delete[] m_pBuffer;
            m_pBuffer = NULL;
        }
        __except(1)
        {
            m_pBuffer = NULL;
        }
        m_pBuffer = NULL;
    }
    m_ulBufferSize = 0;
    m_ulReader = 0;
    m_ulWriter = 0;
    m_ulDataSize = 0;
    ::LeaveCriticalSection(&m_cs);

    ::DeleteCriticalSection(&m_cs);
}

//���û�������С������������ɺ󻺳�Ĵ�С
unsigned long CRingCache::SetCacheSize(unsigned long ulCacheSize)
{
    ::EnterCriticalSection(&m_cs);
    //�������
    m_ulReader = 0;
    m_ulWriter = 0;
    m_ulDataSize = 0;

    //��������Сδ�����仯������Ҫ���������ڴ�
    if(ulCacheSize == m_ulBufferSize)
    {
        ::LeaveCriticalSection(&m_cs);
        return m_ulBufferSize;
    }

    //��������С�����仯����Ҫ���������ڴ�
    //�ͷŵ�ǰ�����ڴ�
    if(NULL != m_pBuffer)
    {
        __try
        {
            delete[] m_pBuffer;
            m_pBuffer = NULL;
        }
        __except(1)
        {
            m_pBuffer = NULL;
        }
        m_pBuffer = NULL;
    }

    //�����»����ڴ�
    m_ulBufferSize = ulCacheSize;
    if(m_ulBufferSize > 0)
    {
        __try
        {
            m_pBuffer = new char[m_ulBufferSize];
        }
        __except(1)
        {
        }

        if(NULL == m_pBuffer)
        {//����ʧ��
            m_ulBufferSize = 0;
        }
    }

    ::LeaveCriticalSection(&m_cs);
    return m_ulBufferSize;
}

//��õ�ǰ��������С
unsigned long CRingCache::GetCacheSize() const
{
    return m_ulBufferSize;
}

//�鿴ָ���������ݣ�����������Ȼ������Щ���ݣ�����ʵ�ʶ�ȡ���ݳ���
//PCLINTע��˵�����ú����ڱ����ڲ���ʹ��
unsigned long CRingCache::Peek(char* pBuf, unsigned long ulPeekLen)/*lint !e1714*/
{
    unsigned long ulResult = 0;

    ::EnterCriticalSection(&m_cs);

    //����ʵ�ʿɶ�ȡ�ĳ���
    ulResult = m_ulDataSize>ulPeekLen?ulPeekLen:m_ulDataSize;
    if(0 == ulResult)
    {
        ::LeaveCriticalSection(&m_cs);
        return ulResult;
    }
    
    //���ݳʵ��ηֲ�
    if(m_ulReader < m_ulWriter)
    {//ooo********ooooo
        eSDK_MEMCPY(pBuf, ulResult, m_pBuffer+m_ulReader, ulResult);
        ::LeaveCriticalSection(&m_cs);
        return ulResult;
    }

    //���ݳ����ηֲ���m_ulReader����m_ulWriterʱ��������Ҳ������
    //*B*oooooooo**A**    
    unsigned long ulASectionLen = m_ulBufferSize - m_ulReader;//A�����ݳ���
    if(ulResult <= ulASectionLen)//A�����ݳ����㹻
    {
        eSDK_MEMCPY(pBuf, ulResult, m_pBuffer+m_ulReader, ulResult);
    }
    else//A�����ݳ��Ȳ���������Ҫ��B�ζ�ȡ
    {
        //�ȶ�A�Σ��ٴ�B�β���
        eSDK_MEMCPY(pBuf, ulASectionLen, m_pBuffer+m_ulReader, ulASectionLen);
        eSDK_MEMCPY(pBuf+ulASectionLen, ulResult-ulASectionLen, m_pBuffer, ulResult-ulASectionLen);
    }
    
    ::LeaveCriticalSection(&m_cs);
    return ulResult;
}

//��ȡָ���������ݣ�����ʵ�ʶ�ȡ���ݳ���
unsigned long CRingCache::Read(char* pBuf, unsigned long ulReadLen)
{
    unsigned long ulResult = 0;

    ::EnterCriticalSection(&m_cs);
    
    //����ʵ�ʿɶ�ȡ�ĳ���
    ulResult = m_ulDataSize>ulReadLen?ulReadLen:m_ulDataSize;
    if(0 == ulResult)
    {
        ::LeaveCriticalSection(&m_cs);
        return ulResult;
    }
    
    //���ݳʵ��ηֲ�
    if(m_ulReader < m_ulWriter)
    {//ooo********ooooo
        eSDK_MEMCPY(pBuf, ulResult, m_pBuffer+m_ulReader, ulResult);

        //���ݱ���ȡ�����¶�ȡλ��
        m_ulReader += ulResult;
        m_ulReader %= m_ulBufferSize;
        //�����ѱ���ȡ�����»��������ݳ���
        m_ulDataSize -= ulResult;

        ::LeaveCriticalSection(&m_cs);
        return ulResult;
    }

    //���ݳ����ηֲ���m_ulReader����m_ulWriterʱ��������Ҳ������
    //*B*oooooooo**A**
    unsigned long ulASectionLen = m_ulBufferSize - m_ulReader;//A�����ݳ���
    if(ulResult <= ulASectionLen)//A�����ݳ����㹻
    {
        eSDK_MEMCPY(pBuf, ulResult, m_pBuffer+m_ulReader, ulResult);

        //���ݱ���ȡ�����¶�ȡλ��
        m_ulReader += ulResult;
        m_ulReader %= m_ulBufferSize;
    }
    else//A�����ݳ��Ȳ���������Ҫ��B�ζ�ȡ
    {
        //�ȶ�A�Σ��ٴ�B�β���
        eSDK_MEMCPY(pBuf, ulASectionLen, m_pBuffer+m_ulReader, ulASectionLen);
        eSDK_MEMCPY(pBuf+ulASectionLen, ulResult-ulASectionLen, m_pBuffer, ulResult-ulASectionLen);
        m_ulReader = ulResult - ulASectionLen;//���ݱ���ȡ�����¶�ȡλ��
    }
    //�����ѱ���ȡ�����»��������ݳ���
    m_ulDataSize -= ulResult;
    
    ::LeaveCriticalSection(&m_cs);
    return ulResult;
}

//дָ���������ݣ�����ʵ��д���ݳ��ȣ����������ռ䲻������ֹд��
unsigned long CRingCache::Write(const char* pBuf, unsigned long ulWriteLen)
{
    unsigned long ulResult = 0;

    ::EnterCriticalSection(&m_cs);
    
    //����ʵ�ʿ�д�볤�ȣ������໺������������д������
    ulResult = (m_ulBufferSize-m_ulDataSize)<ulWriteLen?0:ulWriteLen;
    if(0 == ulResult)
    {
        ::LeaveCriticalSection(&m_cs);
        return ulResult;
    }

    //����ռ�ʵ��ηֲ�
    if(m_ulReader > m_ulWriter)
    {//***oooooooo*****
        eSDK_MEMCPY(m_pBuffer+m_ulWriter, ulResult, pBuf, ulResult);

        //������д�룬����д��λ��
        m_ulWriter += ulResult;
        m_ulWriter %= m_ulBufferSize;
        //������д�룬���»��������ݳ���
        m_ulDataSize += ulResult;

        ::LeaveCriticalSection(&m_cs);
        return ulResult;
    }
    
    //����ռ�����ηֲ���m_ulReader����m_ulWriterʱ�����ݣ�Ҳ�����ηֲ�
    //oBo********ooAoo
    unsigned long ulASectionLen = m_ulBufferSize - m_ulWriter;//A�ο��໺�峤��
    if(ulResult <= ulASectionLen)//A�ο��໺�峤���㹻
    {
        eSDK_MEMCPY(m_pBuffer+m_ulWriter, ulResult, pBuf, ulResult);

        //������д�룬����д��λ��
        m_ulWriter += ulResult;
        m_ulWriter %= m_ulBufferSize;
    }
    else//A�ο��໺�峤�Ȳ�������Ҫ��B��д��
    {
        eSDK_MEMCPY(m_pBuffer+m_ulWriter, ulASectionLen, pBuf, ulASectionLen);
        eSDK_MEMCPY(m_pBuffer, ulResult-ulASectionLen, pBuf+ulASectionLen, ulResult-ulASectionLen);
        m_ulWriter = ulResult - ulASectionLen;//������д�룬����д��λ��
    }
    
    //������д�룬���»��������ݳ���
    m_ulDataSize += ulResult;

    ::LeaveCriticalSection(&m_cs);
    return ulResult;
}

//��õ�ǰ���������ݴ�С
unsigned long CRingCache::GetDataSize() const
{
    return m_ulDataSize;
}

//��õ�ǰ���໺���С
unsigned long CRingCache::GetEmptySize() const
{
    return (m_ulBufferSize - m_ulDataSize);
}

//�������
void CRingCache::Clear()
{
    ::EnterCriticalSection(&m_cs);
    m_ulReader = 0;
    m_ulWriter = 0;
    m_ulDataSize = 0;
    ::LeaveCriticalSection(&m_cs);
}

//��õ�ǰ�����������ݳ��Ⱥͻ��������ȵı����İٷ���
unsigned long CRingCache::GetUsingPercent() const
{
    //��ֹ����Ϊ0���쳣����
    if (0 == m_ulBufferSize) 
    {
        return 0;
    }
    
    unsigned long ulCurrentUsingPercent = (m_ulDataSize*100)/m_ulBufferSize;
    
    return ulCurrentUsingPercent;
}

//�ָ�PC��LINT���
/*lint +e414*/
/*lint +e429*/
/*lint +e613*/
/*lint +e662*/
/*lint +e668*/
/*lint +e669*/
/*lint +e670*/
/*lint +e826*/
/*lint +e1714*/



