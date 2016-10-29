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

#include "UdpConnection.h"
#include "ivs_log.h"
#include "eSDK_Securec.h"

CUdpConnection::CUdpConnection(void)
{
}


CUdpConnection::~CUdpConnection(void)
{
}

int CUdpConnection::Bind(const char* pszLocalIP, unsigned int uiLocalPort)
{
    int iRet = CConnection::Bind(pszLocalIP, uiLocalPort);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Bind", "Call base class failed.");
        return IVS_FAIL;
    }

    m_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_socket == INVALID_SOCKET)
    {
        IVS_LOG(IVS_LOG_ERR, "Bind", "Create udp socket failed, error:%d",  WSAGetLastError());
        return IVS_FAIL;
    }

    sockaddr_in _sin;
	eSDK_MEMSET(&_sin, 0, sizeof(_sin));
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons((u_short)m_uiLocalPort);
    _sin.sin_addr.s_addr = inet_addr(m_strLocalIP.c_str());

    if(SOCKET_ERROR == ::bind(m_socket,(sockaddr*)&_sin,sizeof(_sin)))
    {
        IVS_LOG(IVS_LOG_ERR, "Bind", "Bind udp socket failed, error:%d",  WSAGetLastError());
        return IVS_FAIL;
    }

    return IVS_SUCCEED;
}

int CUdpConnection::Connect(const char* pszServerIP, unsigned int uiServerPort)
{
    int iRet = CConnection::Connect(pszServerIP, uiServerPort);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_ERR, "Connect", "Call base class failed.");
        return IVS_FAIL;
    }

    sockaddr_in _sin;
	eSDK_MEMSET(&_sin, 0, sizeof(_sin));
    _sin.sin_family = AF_INET;
    _sin.sin_addr.s_addr = inet_addr(m_strServerIP.c_str());
    _sin.sin_port = htons((unsigned short)m_uiServerPort);

    return IVS_SUCCEED;
}

int CUdpConnection::RecvData(char* pData, unsigned int uiDataLen)
{
    if(NULL == pData)
    {
        IVS_LOG(IVS_LOG_ERR, "RecvData", "Input Param error, pData is NULL");
        return -1; // 与系统的recvfrom的错误返回值保持统一;
    }
    sockaddr_in addr;
	eSDK_MEMSET(&addr, 0, sizeof(addr));
    int nLen = sizeof(addr);
    
    int iRet = Select(READ);
    if (iRet <= 0)
    {
       // IVS_LOG(IVS_LOG_ERR, "RecvData", "Select read mode error, error code:%d", WSAGetLastError());
    }
    else
    {
        iRet = ::recvfrom(m_socket, pData, (int)uiDataLen, 0, (sockaddr*)&addr,&nLen);
    }
    return iRet;
}

int CUdpConnection::SendData(const char* pData, unsigned int uiDataLen)
{
    if(NULL == pData)
    {
        IVS_LOG(IVS_LOG_ERR, "SendData", "Input Param error, pData is NULL");
        return -1;  // 与系统的sendto的错误返回值保持统一;
    }
    
    sockaddr_in addr;
	eSDK_MEMSET(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port  = htons((u_short)m_uiServerPort);
    addr.sin_addr.S_un.S_addr = inet_addr(m_strServerIP.c_str());
    
	int iRet = ::sendto(m_socket, pData, (int)uiDataLen, 0, (sockaddr*)&addr, sizeof(addr));
    return iRet;
}

int CUdpConnection::SendMulitCastData(const char* pData, unsigned int uiDataLen, const sockaddr_in& sockAddr)
{
	if(NULL == pData)
	{
		IVS_LOG(IVS_LOG_ERR, "SendData", "Input Param error, pData is NULL");
		return -1;  // 与系统的sendto的错误返回值保持统一;
	}

	int iRet = ::sendto(m_socket, pData, (int)uiDataLen, 0, (sockaddr*)&sockAddr, sizeof(sockAddr));
	return iRet;
}
