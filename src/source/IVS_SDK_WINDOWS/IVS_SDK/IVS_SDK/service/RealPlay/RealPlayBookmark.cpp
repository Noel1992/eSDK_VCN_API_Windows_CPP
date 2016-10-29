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

#include "RealPlayBookmark.h"
#include "UserMgr.h"
#include "IVS_Trace.h"

CRealPlayBookmark::CRealPlayBookmark(void)
{//lint !e1927
	m_UserMgr = NULL;
}


CRealPlayBookmark::~CRealPlayBookmark(void)
{
	m_UserMgr = NULL;
}

void CRealPlayBookmark::SetUserMgr( CUserMgr *pUserMgr)
{
	m_UserMgr = pUserMgr;
}



//新增实况书签;
IVS_INT32 CRealPlayBookmark::AddBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime,IVS_UINT32* uiBookmarkID)const
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pBookmarkName, IVS_PARA_INVALID);
	CHECK_POINTER(pBookmarkTime, IVS_PARA_INVALID);
	CHECK_POINTER(uiBookmarkID, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;
	CXml xmlRsp;

	const CUserInfo* userInfo = m_UserMgr->GetUserInfoEx();
	CHECK_POINTER(userInfo, IVS_OPERATE_MEMORY_ERROR);
	string userName = userInfo->GetLoginName();
	std::string strUserDomainCode;
	m_UserMgr->GetDomainCode(strUserDomainCode);
	(void)CRealXMLProcess::AddBookmarkGetXML(m_UserMgr->GetUserID(),userName.c_str(),strUserDomainCode.c_str(),pDomainCode,pCameraCode,pBookmarkName, pBookmarkTime, xmlReq);//lint !e64 匹配
	unsigned int xmlLen = 0;
	const IVS_CHAR* pReqPlan = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1];
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1];
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	std::string strpRsp;
	IVS_INT32 iRet = m_UserMgr->SendMsg(chDomaCode, NSS_ADD_RECORD_BOOKMARK_REQ, pReqPlan, strpRsp, chDevCode);
	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("Real Play Bookmark: Add Book mark", "ok", "Add BookMark Success.");

		if (xmlRsp.Parse(strpRsp.c_str()))//lint !e64 匹配
		{
			if (!xmlRsp.FindElemEx("Content/RecordBookmark/BookmarkID"))
			{
				BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Response XML", "xml.FindElem(Content/RecordBookmark/BookmarkID) is fail");
				return IVS_XML_INVALID;
			} 
			const char* szElemValue = NULL;
			GET_ELEM_VALUE_NUM_FOR_UINT("BookmarkID",szElemValue, *uiBookmarkID,xmlRsp);
			return iRet;
		}
		else
		{
			BP_RUN_LOG_ERR(IVS_XML_INVALID,"Parse Response XML", "Parse Rsp xml fail.");
			return IVS_XML_INVALID;
		}
	}

	BP_RUN_LOG_ERR(iRet, "Real Play Bookmark", "Add BookMark Failed.");
	return iRet;
}

//修改实况书签;
IVS_INT32 CRealPlayBookmark::ModifyBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName)const
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(pNewBookmarkName, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;

	const CUserInfo* userInfo = m_UserMgr->GetUserInfoEx();
	CHECK_POINTER(userInfo, IVS_OPERATE_MEMORY_ERROR);
	string userName = userInfo->GetLoginName();
	std::string strUserDomainCode;
	m_UserMgr->GetDomainCode(strUserDomainCode);
	(void)CRealXMLProcess::ModifyBookmarkGetXML(m_UserMgr->GetUserID(),userName.c_str(),strUserDomainCode.c_str(),pDomainCode,pCameraCode,uiBookmarkID, pNewBookmarkName, xmlReq);//lint !e64 匹配
	unsigned int xmlLen = 0;
	const IVS_CHAR* pModifyBookmarkReq = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pModifyBookmarkReq, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	std::string strpRsp;
	IVS_INT32 iRet = m_UserMgr->SendMsg(chDomaCode, NSS_MOD_RECORD_BOOKMARK_REQ, pModifyBookmarkReq, strpRsp, chDevCode);
	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("Real Play Bookmark: Modify Book mark", "ok", "Modify BookMark Success.");
		return iRet;
	}

	BP_RUN_LOG_ERR(iRet, "Real Play Bookmark", "Modify BookMark Failed.");
	return iRet;
}

//删除实况书签;
IVS_INT32 CRealPlayBookmark::DeleteBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID)const
{
	CHECK_POINTER(pDomainCode, IVS_PARA_INVALID);
	CHECK_POINTER(pCameraCode, IVS_PARA_INVALID);
	CHECK_POINTER(m_UserMgr, IVS_OPERATE_MEMORY_ERROR);
	IVS_DEBUG_TRACE("");

	//数据结构拼装XML并转换为const IVS_CHAR *
	CXml xmlReq;

	(void)CRealXMLProcess::DeleteBookmarkGetXML(pDomainCode,pCameraCode,uiBookmarkID, xmlReq);
	unsigned int xmlLen = 0;
	const IVS_CHAR* pReqPlan = xmlReq.GetXMLStream(xmlLen);
	CHECK_POINTER(pReqPlan, IVS_OPERATE_MEMORY_ERROR);

	IVS_CHAR chDevCode[IVS_DEV_CODE_LEN+1] = {0};
	IVS_CHAR chDomaCode[IVS_DOMAIN_CODE_LEN+1] = {0};
	(IVS_VOID)CXmlProcess::ParseDevCode(pCameraCode, chDevCode, chDomaCode);

	std::string strpRsp;
	IVS_INT32 iRet = m_UserMgr->SendMsg(chDomaCode, NSS_DEL_RECORD_BOOKMARK_REQ, pReqPlan, strpRsp, chDevCode);
	if(IVS_SUCCEED == iRet)
	{
		BP_RUN_LOG_INF("Real Play Bookmark: Delete Book mark", "ok", "Delete BookMark Success.");
		return iRet;
	}

	BP_RUN_LOG_ERR(iRet, "Real Play Bookmark", "Delete BookMark Failed.");
	return iRet;
}

