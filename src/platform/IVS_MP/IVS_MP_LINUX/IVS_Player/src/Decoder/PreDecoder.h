/*******************************************************************************
//  版权所有    2011 华为技术有限公司
//  程 序 集：  IVS_Player
//  文 件 名：  PreDecoder.h
//  文件说明:   预解码函数定义
//  版    本:   eIVS V100R001C21
//  作    者:   gaojiuwen/g00164844
//  创建日期:   2012/01/20
//
//  修改标识：
//  修改说明：
*******************************************************************************/
// #include "PlayerTypes.h" 
#ifndef _C_PREDECODER_H_HEADER_INCLUDE_
#define _C_PREDECODER_H_HEADER_INCLUDE_

#pragma pack(push, 1)

//视频流参考帧、宏块宽高信息
typedef struct tagFrameHeader
{
    double        dSpeed;
    unsigned int  uRenderTimerInterval;
    unsigned int  uRefFrameNum;
    unsigned int  uMBWidth;
    unsigned int  uMBHeight;
    unsigned int  uPayLoad;
    unsigned int  uWaterMarkValue;
    unsigned int  uTimeStampForWaterMark;
    bool          bSpecialProc;
    bool          bCrop;//是否需要裁剪，目前仅支持裁剪底部
    unsigned int  uCropBottomOffset;//目前仅支持裁剪底部，一般为8
    bool          bIsIDR;
    unsigned int  CurrentTime; //本地文件回放的时间
    bool          bIsLocalPlay;
}FRAME_HEADER, pFRAME_HEADER;

//SPS、PPS数据
typedef struct
{
    unsigned char* p_start;
    unsigned char* p;
    unsigned char* p_end;

    int     i_left;    /* i_count number of available bits */
    int     i_bits_encoded; /* RD only */
} BS_T;

//SPS数据类型
typedef struct
{
    int i_id;

    int i_profile_idc;
    int i_level_idc;

    int b_constraint_set0;
    int b_constraint_set1;
    int b_constraint_set2;

    int i_log2_max_frame_num;

    int i_poc_type;
    /* poc 0 */
    int i_log2_max_poc_lsb;
    /* poc 1 */
    int b_delta_pic_order_always_zero;
    int i_offset_for_non_ref_pic;
    int i_offset_for_top_to_bottom_field;
    int i_num_ref_frames_in_poc_cycle;
    int i_offset_for_ref_frame[256];

    int i_num_ref_frames;
    int b_gaps_in_frame_num_value_allowed;
    int i_mb_width;
    int i_mb_height;
    int b_frame_mbs_only;
    int b_mb_adaptive_frame_field;
    int b_direct8x8_inference;

    int b_crop;
    struct
    {
        int i_left;
        int i_right;
        int i_top;
        int i_bottom;
    } crop;

    int b_vui;
    struct
    {
        int b_aspect_ratio_info_present;
        int i_sar_width;
        int i_sar_height;

        int b_overscan_info_present;
        int b_overscan_info;

        int b_signal_type_present;
        int i_vidformat;
        int b_fullrange;
        int b_color_description_present;
        int i_colorprim;
        int i_transfer;
        int i_colmatrix;

        int b_chroma_loc_info_present;
        int i_chroma_loc_top;
        int i_chroma_loc_bottom;

        int b_timing_info_present;
        int i_num_units_in_tick;
        int i_time_scale;
        int b_fixed_frame_rate;

        int b_bitstream_restriction;
        int b_motion_vectors_over_pic_boundaries;
        int i_max_bytes_per_pic_denom;
        int i_max_bits_per_mb_denom;
        int i_log2_max_mv_length_horizontal;
        int i_log2_max_mv_length_vertical;
        int i_num_reorder_frames;
        int i_max_dec_frame_buffering;

        /* FIXME to complete */
    } vui;

    int b_qpprime_y_zero_transform_bypass;

} H264_SPS_T;

#pragma pack(pop)

//根据SPS获取参考帧、宏块宽高信息(已经过加1处理)
int GetSPSInfo(unsigned char* pBuffer, unsigned int uBufLen, FRAME_HEADER* pInfo);
//SPS参数解析
int SPSRead(BS_T* s, H264_SPS_T* sps_array);//H264_SPS_T sps_array[32]

void bs_init( BS_T* s, void* p_data, int i_data );
int bs_pos( BS_T* s );
int bs_eof( BS_T* s );
unsigned bs_read( BS_T* s, int i_count );
unsigned bs_read1( BS_T* s );
void bs_skip( BS_T* s, int i_count );
int bs_read_ue( BS_T* s );
int bs_read_se( BS_T* s );
int bs_read_te( BS_T* s, int x );

void seq_scaling_matrix_present_proc(BS_T *&s, int i_chroma_format_idc);
void check_profile_idc(BS_T *&s, int i_profile_idc, int &i_chroma_format_idc);

#endif // end of _C_PREDECODER_H_HEADER_INCLUDE_
