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
	filename	: 	RealPlayBookmark.h
	author		:	z00201790
	created		:	2012/12/06	
	description	:	����¼����ǩ������;
	copyright	:	Copyright (C) 2011-2015
	history		:	2012/12/06 ��ʼ�汾;
*********************************************************************/

#ifndef __REALPLAY_BOOKMARK_H__
#define __REALPLAY_BOOKMARK_H__

#include "RealXMLProcess.h"

class CUserMgr;
class CRealPlayBookmark
{
public:
	CRealPlayBookmark(void);
	~CRealPlayBookmark(void);
	void SetUserMgr(CUserMgr *pUserMgr);

	/***********************************************************************************
    * name       : AddBookmark
    * description: ����ʵ����ǩ
    * input      : pCameraCode:���������; pBookmarkTime:��ǩʱ��; pNewBookmarkName:����ǩ��
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 AddBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,const IVS_CHAR* pBookmarkName,const IVS_CHAR* pBookmarkTime,IVS_UINT32* uiBookmarkID)const;

	/***********************************************************************************
    * name       : ModifyBookmark
    * description: �޸�ʵ����ǩ
    * input      :pCameraCode:���������; uiBookmarkID:��ǩID; pNewBookmarkName:����ǩ��
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 ModifyBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID,const IVS_CHAR* pNewBookmarkName)const;

	/***********************************************************************************
    * name       : DeleteBookmark
    * description: ɾ��ʵ����ǩ
    * input      : pCameraCode:���������; uiBookmarkID:��ǩID
    * output     : NA
    * return     : �ɹ�����0��ʧ�ܷ��ظ���������
    * remark     : NA
    ***********************************************************************************/
	IVS_INT32 DeleteBookmark(const IVS_CHAR* pDomainCode,const IVS_CHAR* pCameraCode,IVS_UINT32 uiBookmarkID)const;
	

private:
	CUserMgr *m_UserMgr;
};
#endif
