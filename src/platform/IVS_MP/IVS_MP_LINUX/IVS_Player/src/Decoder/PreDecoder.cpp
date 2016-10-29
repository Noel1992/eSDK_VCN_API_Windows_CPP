/*******************************************************************************
//  版权所有    2011 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  PreDecoder.c
//  文件说明:   预解码函数实现
//  版    本:   eIVS V100R001C21
//  作    者:   gaojiuwen/g00164844
//  创建日期:   2012/01/20
//
//  修改标识：
//  修改说明：
*******************************************************************************/
// #include "PlayerCommon.h"
// #include "PlayerDataType.h"
#include "PreDecoder.h"
#include "IVSPlayerDataType.h"
#include "ivs_log.h"

//根据SPS获取参考帧、宏块宽高信息(已经过加1处理)
int GetSPSInfo(unsigned char* pBuffer, unsigned int uBufLen, FRAME_HEADER* pInfo)
{
    if ((NULL == pInfo) || (NULL == pBuffer) || (0 == uBufLen))
    {
        IVS_LOG(IVS_LOG_DEBUG, "GetSPSInfo", "Input H264 stream info pointer  is NULL.");
        return IVS_PARA_INVALID;
    }

    BS_T spsData;
    H264_SPS_T h264SPS;
    memset(&spsData, 0, sizeof(BS_T));
    memset(&h264SPS, 0, sizeof(H264_SPS_T));

    //将SPS二进制码流转换为解析用的BS_T结构
    bs_init(&spsData, pBuffer, uBufLen);                   //lint !e713

    //对SPS二进制码流进行指数哥伦布解码
    int iRet = SPSRead(&spsData, &h264SPS);
    if (IVS_SUCCEED != iRet)
    {
        IVS_LOG(IVS_LOG_DEBUG, "GetSPSInfo", "Decode SPS failed, Result code is %d.", iRet);
        return iRet;
    }

    pInfo->uRefFrameNum = (unsigned int)(h264SPS.i_num_ref_frames);
    pInfo->uMBWidth     = (unsigned int)(h264SPS.i_mb_width);

    if (!h264SPS.b_frame_mbs_only)//可能是场编码
    {
        pInfo->uMBHeight = (unsigned int)(h264SPS.i_mb_height) * 2;
        pInfo->bSpecialProc = true;
    }
    else
    {
        pInfo->uMBHeight = (unsigned int)(h264SPS.i_mb_height);
        pInfo->bSpecialProc = false;
    }

    pInfo->bCrop = (h264SPS.b_crop == 1);
    if (pInfo->bCrop)
    {
        pInfo->uCropBottomOffset = (unsigned int)h264SPS.crop.i_bottom;
    }

    return IVS_SUCCEED;      
}

int SPSRead( BS_T* s, H264_SPS_T* sps_array)
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_DEBUG, "SPSRead", "Input parameter is NULL.");
        return IVS_PARA_INVALID;
    }

    if (NULL == sps_array)
    {
        IVS_LOG(IVS_LOG_DEBUG, "SPSRead", "Input parameter is NULL.");
        return IVS_PARA_INVALID;
    }

    int i_profile_idc;
    int i_level_idc;

    int b_constraint_set0;
    int b_constraint_set1;
    int b_constraint_set2;

    int id;

    int i_chroma_format_idc = 1;//色度取样,值应该在0到3的范围内(包括0和3)。
                                //当chroma_format_idc不存在时，应推断其值为1（4：2：0的色度格式）

    i_profile_idc     = bs_read( s, 8 );   //lint !e713
    b_constraint_set0 = bs_read( s, 1 );   //lint !e713
    b_constraint_set1 = bs_read( s, 1 );   //lint !e713
    b_constraint_set2 = bs_read( s, 1 );   //lint !e713

    bs_skip( s, 5 );    /* reserved */
    i_level_idc       = bs_read( s, 8 );     //lint !e713

    id = bs_read_ue( s );
    if( bs_eof( s ) || id >= 32 )
    {
        /* the sps is invalid, no need to corrupt sps_array[0] */
        return IVS_FAIL;
    }

    check_profile_idc(s, i_profile_idc, i_chroma_format_idc);

    sps_array->i_id              = id;
    sps_array->i_profile_idc     = i_profile_idc;
    sps_array->i_level_idc       = i_level_idc;
    sps_array->b_constraint_set0 = b_constraint_set0;
    sps_array->b_constraint_set1 = b_constraint_set1;
    sps_array->b_constraint_set2 = b_constraint_set2;

    sps_array->i_log2_max_frame_num = bs_read_ue( s ) + 4;

    sps_array->i_poc_type = bs_read_ue( s );

    if( sps_array->i_poc_type == 0 )
    {
        sps_array->i_log2_max_poc_lsb = bs_read_ue( s ) + 4;
    }
    else if( sps_array->i_poc_type == 1 )
    {
        int i;
        sps_array->b_delta_pic_order_always_zero = bs_read( s, 1 );    //lint !e713
        sps_array->i_offset_for_non_ref_pic = bs_read_se( s );
        sps_array->i_offset_for_top_to_bottom_field = bs_read_se( s );
        sps_array->i_num_ref_frames_in_poc_cycle = bs_read_ue( s );
        if( sps_array->i_num_ref_frames_in_poc_cycle > 256 )
        {
            /* FIXME what to do */
            sps_array->i_num_ref_frames_in_poc_cycle = 256;
        }
        for( i = 0; i < sps_array->i_num_ref_frames_in_poc_cycle; i++ )
        {
            sps_array->i_offset_for_ref_frame[i] = bs_read_se( s );
        }
    }
    else if( sps_array->i_poc_type > 2 )
    {
        sps_array->i_id = -1;
        return IVS_FAIL;
    }

    sps_array->i_num_ref_frames = bs_read_ue( s );
    sps_array->b_gaps_in_frame_num_value_allowed = bs_read( s, 1 );   //lint !e713
    sps_array->i_mb_width = bs_read_ue( s ) + 1;
    sps_array->i_mb_height = bs_read_ue( s ) + 1;

    //b_frame_mbs_only等于0表示编码视频序列的编码图像可能是编码场或编码帧。
    //b_frame_mbs_only等于1表示编码视频序列的每个编码图像都是一个仅包含帧宏块的编码帧。
    sps_array->b_frame_mbs_only = bs_read( s, 1 );//lint !e713
    if(!sps_array->b_frame_mbs_only)
    {
        //mb_adaptive_frame_field_flag,等于0表示在一个图像的帧和场宏块之间没有交换。
        //mb_adaptive_frame_field_flag 等于1表示在帧和帧内的场宏块之间可能会有交换。默认为0
        sps_array->b_mb_adaptive_frame_field = bs_read( s, 1 );//lint !e713
    }

    /* b_direct8x8_inference */
    bs_skip( s, 1 );

    //帧剪切偏移参数
    sps_array->b_crop = (int)bs_read( s, 1 );
    if( sps_array->b_crop )
    {
        /* left */
        sps_array->crop.i_left = bs_read_ue( s );
        /* right */
        sps_array->crop.i_right = bs_read_ue( s );
        /* top */
        sps_array->crop.i_top = bs_read_ue( s );
        /* bottom */
        sps_array->crop.i_bottom = bs_read_ue( s );

        //计算最终需要裁剪掉的大小，目前仅支持底部
        sps_array->crop.i_bottom = ((i_chroma_format_idc == 1) ? 2 : 1) * (2-sps_array->b_frame_mbs_only) * sps_array->crop.i_bottom;
    }

    /* vui */
    sps_array->b_vui = (int)bs_read( s, 1 );

    return IVS_SUCCEED;//lint !e438
}

void check_profile_idc(BS_T *&s, int i_profile_idc, int &i_chroma_format_idc)
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_DEBUG, "SPSRead", "Input parameter is NULL.");
        return;
    }

//     int i_bit_depth_luma_minus8;
//     int i_bit_depth_chroma_minus8;
//     int b_qpprime_y_zero_transform_bypass_flag;
    int b_seq_scaling_matrix_present_flag;

//     int b_residual_colour_transform_flag;

    if( i_profile_idc == 100 || i_profile_idc == 110 ||
        i_profile_idc == 122 || i_profile_idc == 144 ) 
    {
        i_chroma_format_idc = bs_read_ue( s );

        if(i_chroma_format_idc == 3)
        {
            //b_residual_colour_transform_flag = bs_read( s, 1 );
            (void)bs_read( s, 1 );
        }
//         i_bit_depth_luma_minus8 = bs_read_ue( s );
        (void)bs_read_ue( s );
//         i_bit_depth_chroma_minus8 = bs_read_ue( s );
        (void)bs_read_ue( s );
//         b_qpprime_y_zero_transform_bypass_flag = bs_read( s, 1 ); //lint !e713
        (void)bs_read( s, 1 );
        b_seq_scaling_matrix_present_flag = bs_read( s, 1 ); //lint !e713

        if( b_seq_scaling_matrix_present_flag )
        {
            seq_scaling_matrix_present_proc(s, b_seq_scaling_matrix_present_flag);
        }
    }
}

void seq_scaling_matrix_present_proc(BS_T *&s, int i_chroma_format_idc)
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_DEBUG, "SPSRead", "Input parameter is NULL.");
        return;
    }

    int b_seq_scaling_list_present_flag[12];

    for( int i = 0; i < ((3 != i_chroma_format_idc) ? 8 : 12); i++ )
    {
        b_seq_scaling_list_present_flag[i] = bs_read( s, 1 );//lint !e713
        if( !b_seq_scaling_list_present_flag[i] )
            continue;
        const int i_size_of_scaling_list = (i < 6 ) ? 16 : 64;
        /* scaling_list (...) */
        int i_lastscale = 8;
        int i_nextscale = 8;
        int i_tmp;
        for( int j = 0; j < i_size_of_scaling_list; j++ )
        {
            if( i_nextscale != 0 )
            {
                /* delta_scale */
                i_tmp = bs_read_se( s );
                i_nextscale = ( i_lastscale + i_tmp + 256 ) % 256;
                /* useDefaultScalingMatrixFlag = ... */
            }
            /* scalinglist[j] */
            i_lastscale = ( i_nextscale == 0 ) ? i_lastscale : i_nextscale;
        }
    }
}

void bs_init( BS_T* s, void* p_data, int i_data )
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_DEBUG, "bs_init", "Input parameter is NULL.");
        return ;
    }

    s->p_start = (unsigned char*)p_data;
    s->p       = (unsigned char*)p_data;
    s->p_end   = s->p + i_data;
    s->i_left  = 8;
}


int bs_pos( BS_T* s )
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_ERR, "bs_pos", "Input parameter is NULL.");
        return IVS_PARA_INVALID;
    }

    return( 8 * ( s->p - s->p_start ) + 8 - s->i_left );
}//lint !e818

int bs_eof( BS_T* s )
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_ERR, "bs_eof", "Input parameter is NULL.");
        return IVS_PARA_INVALID;
    }

    return( s->p >= s->p_end ? 1: 0 );
}//lint !e818


unsigned bs_read( BS_T* s, int i_count )
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_ERR, "bs_read", "Input parameter is NULL.");
        return IVS_PARA_INVALID;
    }

    static unsigned i_mask[33] ={0x00,
        0x01,      0x03,      0x07,      0x0f,
        0x1f,      0x3f,      0x7f,      0xff,
        0x1ff,     0x3ff,     0x7ff,     0xfff,
        0x1fff,    0x3fff,    0x7fff,    0xffff,
        0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
        0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
        0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
        0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};
    int      i_shr;
    unsigned i_result = 0;

    while( i_count > 0 )
    {
        if( s->p >= s->p_end )
        {
            break;
        }

        if( ( i_shr = s->i_left - i_count ) >= 0 )
        {
            /* more in the buffer than requested */
            i_result |= ( *s->p >> i_shr )&i_mask[i_count];
            s->i_left -= i_count;
            if( s->i_left == 0 )
            {
                s->p++;
                s->i_left = 8;
            }
            return( i_result );
        }
        else
        {
            /* less in the buffer than requested */
            i_result |= (*s->p&i_mask[s->i_left]) << abs(i_shr);
            i_count  -= s->i_left;
            s->p++;
            s->i_left = 8;
        }
    }

    return( i_result );
}

unsigned bs_read1( BS_T* s )
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_ERR, "bs_read1", "Input parameter is NULL.");
        return IVS_PARA_INVALID;
    }

    if( s->p < s->p_end )
    {
        unsigned int i_result;

        s->i_left--;
        i_result = ( *s->p >> s->i_left )&0x01;
        if( s->i_left == 0 )
        {
            s->p++;
            s->i_left = 8;
        }
        return i_result;
    }

    return 0;
}

void bs_skip( BS_T* s, int i_count )
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_ERR, "bs_skip", "Input parameter is NULL.");
        return ;
    }

    s->i_left -= i_count;

    while( s->i_left <= 0 )
    {
        s->p++;
        s->i_left += 8;
    }
}

int bs_read_ue( BS_T* s )
{
    if (NULL == s)
    {
        IVS_LOG(IVS_LOG_ERR, "bs_read_ue", "Input parameter is NULL.");
        return IVS_PARA_INVALID;
    }

    int i = 0;

    while( bs_read1( s ) == 0 && s->p < s->p_end && i < 32 )
    {
        i++;
    }

    return( (( 1 << i) - 1) + bs_read( s, i ) );             //lint !e701 !e713
}

int bs_read_se( BS_T* s )
{
    int val = bs_read_ue( s );

    return val&0x01 ? (val+1)/2 : -(val/2);
}

int bs_read_te( BS_T* s, int x )
{
    if( x == 1 )
    {
        return 1 - bs_read1( s );    //lint !e713
    }
    else if( x > 1 )
    {
        return bs_read_ue( s );
    }
    return 0;
}
