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
	filename	: 	RecordBookmark.h
	author		:	z00201790
	created		:	2012/12/06	
	description	:	����¼����ǩ������;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 ��ʼ�汾;
*********************************************************************/

#ifndef __RECORD_BOOKMARK_H__
#define __RECORD_BOOKMARK_H__

#include "RecordXMLProcess.h"

class CUserMgr;
class CRecordBookmark
{
public:
	CRecordBookmark(void);
	~CRecordBookmark(void);
	void SetUserMgr(CUserMgr *pUserMgr);

	/***********************************************************************************
    * name       : AddBookmark
    * description: ����¼����ǩ
    * input      : pCameraCode:���������; pBookmarkTime:��ǩʱ��; pNewBookmarkName:����ǩ��
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 AddBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime,IVS_UINT32* uiBookmarkID)const;

	/***********************************************************************************
    * name       : ModifyBookmark
    * description: �޸�¼����ǩ
    * input      :pCameraCode:���������; uiBookmarkID:��ǩID; pNewBookmarkName:����ǩ��
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 ModifyBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName)const;

	/***********************************************************************************
    * name       : DeleteBookmark
    * description: ɾ��¼����ǩ
    * input      : pCameraCode:���������; uiBookmarkID:��ǩID
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 DeleteBookmark(IVS_INT32 iRecordMethod,const IVS_CHAR* pDomainCode,const IVS_CHAR* pNVRCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID)const;
	
	/***********************************************************************************
    * name       : GetBookmarkList
    * description: ��ѯ¼����ǩ�б�
    * input      : pUnifiedQuery:��ѯ����; pBookmarkList:��ǩ��ѯ����б�; uiBufSize:�����С
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetBookmarkList(IVS_INT32 iRecordMethod,const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiBufSize)const;

	/***********************************************************************************
    * name       : GetBookmarkListByBackupServer
    * description: ��ѯ¼����ǩ�б�(�ӱ��ݷ�����)
    * input      : pUnifiedQuery:��ѯ����; pBookmarkList:��ǩ��ѯ����б�; uiBufSize:�����С
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 GetBookmarkListByBackupServer(const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiMaxBufSize)const;

private:
	IVS_INT32 GetBookmarkListByBackupServerForOneDomain(IVS_UINT32 uiFromIndex,IVS_UINT32 uiToIndex,const char* cNVRCode,const char* cMBUDomainCode,
		const IVS_QUERY_UNIFIED_FORMAT* pUnifiedQuery,IVS_RECORD_BOOKMARK_INFO_LIST* pBookmarkList,IVS_UINT32 uiMaxBufSize)const;
private:
	CUserMgr *m_UserMgr;

};

#endif
