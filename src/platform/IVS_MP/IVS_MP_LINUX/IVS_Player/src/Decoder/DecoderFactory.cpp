/*******************************************************************************
//  版权所有    2012 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  DecoderFactory.h
//  文件说明:   解码器工厂类
//  版    本:   IVS V100R001C02
//  作    者:   z90003203
//  创建日期:   2012/10/31
//  修改标识：
//  修改说明： 
*******************************************************************************/

#include "DecoderFactory.h"

/*************************************************
Function:        DecoderFactory::DecoderFactory
Description:     构造函数
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
Others:          // 其它说明
*************************************************/
DecoderFactory::DecoderFactory(void)
{
}
/*************************************************
Function:        DecoderFactory::DecoderFactory
Description:     析构函数
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
Others:          // 其它说明
*************************************************/
DecoderFactory::~DecoderFactory(void)
{
}

/*************************************************
Function:        DecoderFactory::createDecoder
Description:     创建一个解码对象
Calls:           // 被本函数调用的函数清单（建议描述）
Called By:       // 调用本函数的函数清单（建议描述）
Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
Output:          // 对输出参数的说明
Return:          // 函数返回值的说明
                    得到一个解码对象指针
Others:          // 其它说明
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
// Description:     得到一个解码工厂类
// Calls:           // 被本函数调用的函数清单（建议描述）
// Called By:       // 调用本函数的函数清单（建议描述）
// Input:           // 输入参数说明，包括每个参数的作用、取值说明及参数间关系。
// Output:          // 对输出参数的说明
// Return:          // 函数返回值的说明
//                     得到一个解码工厂对象指针
// Others:          // 其它说明
// *************************************************/
// DecoderFactory* DecoderFactory::Instance(void)
// {
// 	static DecoderFactory objFactory;
// 	return &objFactory;
// }
