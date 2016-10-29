/*******************************************************************************
//  ��Ȩ����    2012 ��Ϊ�������޹�˾
//  �� �� ����  IVS_Player
//  �� �� ����  DecoderFactory.h
//  �ļ�˵��:   ������������
//  ��    ��:   IVS V100R001C02
//  ��    ��:   z90003203
//  ��������:   2012/10/31
//  �޸ı�ʶ��
//  �޸�˵���� 
*******************************************************************************/

#include "DecoderFactory.h"

/*************************************************
Function:        DecoderFactory::DecoderFactory
Description:     ���캯��
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
Others:          // ����˵��
*************************************************/
DecoderFactory::DecoderFactory(void)
{
}
/*************************************************
Function:        DecoderFactory::DecoderFactory
Description:     ��������
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
Others:          // ����˵��
*************************************************/
DecoderFactory::~DecoderFactory(void)
{
}

/*************************************************
Function:        DecoderFactory::createDecoder
Description:     ����һ���������
Calls:           // �����������õĺ����嵥������������
Called By:       // ���ñ������ĺ����嵥������������
Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
Output:          // �����������˵��
Return:          // ��������ֵ��˵��
                    �õ�һ���������ָ��
Others:          // ����˵��
*************************************************/
CDecoderIVS* DecoderFactory::CreateDecoder()
{
	CDecoderIVS* pDecoder = NULL;

    try
    {
        pDecoder = new CDecoderIVS();
    }
    catch (...)
    {
        if (pDecoder)
        {
            delete pDecoder;
            pDecoder = NULL;
        }

//         IVS_LOG("new CPrivateVideoDecoder throw Exception!");
        return NULL;
    }

	return pDecoder;
}

// /*************************************************
// Function:        DecoderFactory::getInstance
// Description:     �õ�һ�����빤����
// Calls:           // �����������õĺ����嵥������������
// Called By:       // ���ñ������ĺ����嵥������������
// Input:           // �������˵��������ÿ�����������á�ȡֵ˵�����������ϵ��
// Output:          // �����������˵��
// Return:          // ��������ֵ��˵��
//                     �õ�һ�����빤������ָ��
// Others:          // ����˵��
// *************************************************/
// DecoderFactory* DecoderFactory::Instance(void)
// {
// 	static DecoderFactory objFactory;
// 	return &objFactory;
// }
