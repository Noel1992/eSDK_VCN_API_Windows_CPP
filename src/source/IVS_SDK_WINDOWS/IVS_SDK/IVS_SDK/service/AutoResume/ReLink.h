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
	filename	: 	CReLink.cpp
	author		:	z00201790
	created		:	2012/12/27
	description	:	定义重连管理类;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/27 初始版本;
*********************************************************************/

#ifndef RE_LINK_H
#define RE_LINK_H

#include "vos.h"
#include "SDKTimer.h"
#include "SDKDef.h"

class CRelinkInfo
{
public:
    void Init(bool bIsConnected, int iNetElemType, int iPort, const std::string& strLinkID, const std::string& strIP)
    {

        this->m_bIsConnected = bIsConnected;
        this->m_iNetElemType = iNetElemType;
        this->m_iPort = iPort;
        this->m_strLinkID = strLinkID;
        this->m_strIP = strIP;
    };

    CRelinkInfo& operator = (const CRelinkInfo& other)
    {
        if (this == &other)
        {
            return *this;
        }
        this->m_bIsConnected = other.m_bIsConnected;
        this->m_iNetElemType = other.m_iNetElemType;
        this->m_iPort = other.m_iPort;
        this->m_strLinkID = other.m_strLinkID;
        this->m_strIP = other.m_strIP;
        return *this;
    };
    void SetIsConnected(bool bIsConnected){ m_bIsConnected = bIsConnected; };
    bool GetIsConnected() const { return m_bIsConnected; };
    void SetNetElemType(int iNetElemType){ m_iNetElemType = iNetElemType; };
    int GetNetElemType() const { return m_iNetElemType; };
    void SetPort(int iPort) { m_iPort = iPort; };
    int GetPort() const { return m_iPort; };
    void SetLinkID(const std::string& strLinkID) { m_strLinkID = strLinkID; };
    std::string GetLinkID() const { return m_strLinkID; }; //lint !e1036 !e1536
    void SetIP(const std::string& strIP) { m_strIP = strIP; };
    std::string GetIP() const { return m_strIP; }; //lint !e1036 !e1536

private:
    bool m_bIsConnected;
    int  m_iNetElemType;
    int  m_iPort;
	std::string m_strLinkID;
	std::string m_strIP;
};

typedef std::list<CRelinkInfo*> RELINK_LIST;
typedef RELINK_LIST::iterator RELINK_LIST_ITER;

class CUserMgr;
class CReLink: public ITrigger
{
public:
	CReLink(void);
	~CReLink(void);

	virtual void onTrigger(void* /*pArg*/, ULONGLONG /*ullScales*/, TriggerStyle /*style*/);

	void SetUserMgr(CUserMgr *pUserMgr);
public:

	int AddLinkIdToBusinessList(const std::string& strLinkID,int iNetElemType,const std::string& strIP,int iPort);

	//int RemoveLinkIdFromBusinessList(const std::string& strLinkID);

    void ClearBusinessList();

	//判断断线重连的域是否为本域
	bool IsLocalDomain(const CRelinkInfo* pRelink) const;

private:
	RELINK_LIST m_listRelink;

	VOS_Mutex* m_pMutexLock; //lint !e830

	CUserMgr *m_pUserMgr; //lint !e830
};

#endif
