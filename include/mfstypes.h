#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef __MFS_TYPES_H__
#define __MFS_TYPES_H__

#undef  INLINE
#ifdef __cpluscplus
#define INLINE inline
#else
#define INLINE static
#endif

#undef  MIN
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#undef  MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#undef  ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))

typedef enum CtrlInfoTableID         CtrlInfoTableID;     // 控制信息表ID
typedef enum RSCodeRate              RSCodeRate;          // RS码率
typedef enum ByteInterleavedMode     ByteInterleavedMode; // RS码率
typedef enum LDPCCodeRate            LDPCCodeRate;        // LDPC码率
typedef enum ModulationMethod        ModulationMethod;    // 调制方式
typedef enum ScrambleMethod          ScrambleMethod;      // 扰码方式
typedef enum FrameFrequency          FrameFrequency;      // 帧频
typedef enum SampleRate              SampleRate;          // 采样率
typedef enum FrameType               FrameType;           // 帧类型
typedef enum DataUnitType            DataUnitType;        // 数据单元类型
typedef enum EncapsulationMode       EncapsulationMode;   // 复用子帧封装模式
typedef enum ServiceMode             ServiceMode;         // 业务模式

typedef struct NextFrameParam        NextFrameParam;      // 下一帧关键参数
typedef struct MuxFrameHeader        MuxFrameHeader;      // 复用帧头
typedef struct FreqPoint             FreqPoint;           // 频点
typedef struct AdjacentNet           AdjacentNet;         // 邻区网络
typedef struct MuxFrameParam         MuxFrameParam;       // 复用帧参数
typedef struct TimeSlot              TimeSlot;            // 时隙
typedef struct MuxSubFrameParam      MuxSubFrameParam;    // 复用子帧参数
typedef struct Service               Service;             // 业务参数
typedef struct NIT                   NIT;                 // 网络信息表
typedef struct XMCT                  XMCT, CMCT, SMCT;    // 持续/短时间业务复用配置表
typedef struct XSCT                  XSCT, CSCT, SSCT;    // 持续/短时间业务配置表
typedef struct ESGBDT                ESGBDT;              // 电子业务指南基本描述表
typedef struct EB                    EB;                  // 紧急广播
typedef struct EADT                  EADT;                // 加密授权描述表
typedef struct CIDT                  CIDT;                // 公共信息描述表
typedef struct CommonInfo            CommonInfo;          // 公共信息
typedef struct EBDataSection         EBDataSection;       // 紧急广播数据段
typedef struct EBMessage             EBMessage;           // 紧急广播消息
typedef struct EBTrigParam           EBTrigParam;         // 触发参数
typedef struct EBMessageParam        EBMessageParam;      // 消息参数

typedef struct MuxSubFrame           MuxSubFrame;         // 复用子帧
typedef struct MuxSubFrameHeader     MuxSubFrameHeader;   // 复用子帧头
typedef struct VideoSection          VideoSection;        // 视频段
typedef struct AudioSection          AudioSection;        // 音频段
typedef struct DataSection           DataSection;         // 数据段
typedef struct SectionSize           SectionSize;         // 段大小
typedef struct DisplayParam          DisplayParam;        // 显示参数
typedef struct ResolutionParam       ResolutionParam;     // 分辨率参数
typedef struct VideoStreamParam      VideoStreamParam;    // 视频流参数
typedef struct AudioStreamParam      AudioStreamParam;    // 音频流参数
typedef struct VideoUnitParam        VideoUnitParam;      // 视频单元参数
typedef struct AudioUnitParam        AudioUnitParam;      // 音频单元参数
typedef struct DataUnitParam         DataUnitParam;       // 数据单元参数

enum CtrlInfoTableID
{
    CIT_NIT    = 0x01,              // 网络信息表
    CIT_CMCT   = 0x02,              // 持续业务复用配置表
    CIT_CSCT   = 0x03,              // 持续业务配置表
    CIT_SMCT   = 0x04,              // 短时间业务复用配置表
    CIT_SSCT   = 0x05,              // 短时间业务配置表
    CIT_ESGBDT = 0x06,              // ESG基本描述表
    CIT_EADT   = 0x07,              // 加密授权描述表
    CIT_EB     = 0x10,              // 紧急广播
    CIT_CIDT   = 0x11               // 公共信息描述表
};

enum RSCodeRate
{
    RS_CODE_RATE_240_240 = 0x00,    // (240, 240)
    RS_CODE_RATE_240_224,           // (240, 224)
    RS_CODE_RATE_240_192,           // (240, 192)
    RS_CODE_RATE_240_176            // (240, 176)
};

enum ByteInterleavedMode
{
    BYTE_INTERLEAVE_MODE_1 = 0x01,  // 模式1
    BYTE_INTERLEAVE_MODE_2,         // 模式2
    BYTE_INTERLEAVE_MODE_3,         // 模式3
};

enum LDPCCodeRate
{
    LDPC_CODE_RATE_1_2 = 0x00,      // 1/2
    LDPC_CODE_RATE_3_4              // 3/4
};

enum ModulationMethod
{
    MODULATION_METHOD_BPSK = 0x00,  // BPSK
    MODULATION_METHOD_QPSK,         // QPSK
    MODULATION_METHOD_16QAM         // 16QAM
};

enum ScrambleMethod
{
    SCRAMBLE_METHOD_0 = 0x00,       // 模式0
    SCRAMBLE_METHOD_1,              // 模式1
    SCRAMBLE_METHOD_2,              // 模式2
    SCRAMBLE_METHOD_3,              // 模式3
    SCRAMBLE_METHOD_4,              // 模式4
    SCRAMBLE_METHOD_5,              // 模式5
    SCRAMBLE_METHOD_6,              // 模式6
    SCRAMBLE_METHOD_7,              // 模式7
};

enum FrameFrequency
{
    FRAME_FREQUENCY_25HZ = 0x00,    // 25Hz
    FRAME_FREQUENCY_30HZ,           // 30Hz
    FRAME_FREQUENCY_12_5HZ,         // 12.5Hz
    FRAME_FREQUENCY_15HZ,           // 15Hz
};

enum SampleRate
{
    SAMPLE_RATE_8HZ = 0x00,         // 8kHz
    SAMPLE_RATE_12HZ,               // 12kHz
    SAMPLE_RATE_16HZ,               // 16kHz
    SAMPLE_RATE_22_05HZ,            // 22.05kHz
    SAMPLE_RATE_24HZ,               // 24kHz
    SAMPLE_RATE_32HZ,               // 32kHz
    SAMPLE_RATE_44_1HZ,             // 44.1kHz
    SAMPLE_RATE_48HZ,               // 48kHz
    SAMPLE_RATE_96HZ                // 96kHz
};

enum FrameType
{
    FRAME_TYPE_I = 0x00,            // I帧
    FRAME_TYPE_P,                   // P帧
    FRAME_TYPE_B,                   // B帧
};

enum DataUnitType
{
    DATA_UNIT_TYPE_ESG = 0x00,      // ESG数据
    DATA_UNIT_TYPE_PROGRAM,         // 节目提示信息

    DATA_UNIT_TYPE_ECM_128 = 0x80,  // ECM(128), 第一个CAS系统使用
    DATA_UNIT_TYPE_EMM_129,         // EMM(129), 第一个CAS系统使用
    DATA_UNIT_TYPE_ECM_130,         // ECM(128), 第二个CAS系统使用
    DATA_UNIT_TYPE_EMM_131,         // EMM(129), 第二个CAS系统使用
    DATA_UNIT_TYPE_ECM_132,         // ECM(128), 第三个CAS系统使用
    DATA_UNIT_TYPE_EMM_133,         // EMM(129), 第三个CAS系统使用

    DATA_UNIT_TYPE_XPE = 0xA0,      // XPE包
    DATA_UNIT_TYPE_XPE_FEC          // XPE-FEC包
};

enum EncapsulationMode
{
    ENCAPSULATION_MODE2 = 0x00,     // 封装模式2
    ENCAPSULATION_MODE1,            // 封装模式1
};

enum ServiceMode
{
    STREAM_MODE = 0x00,             // 流模式
    FILE_MODE,                      // 文件模式
};

struct NextFrameParam
{
    uint32_t header_length                : 8;
    uint32_t frame1_length                : 24;
    uint8_t frame1_header_length          : 8;
};

struct MuxFrameHeader
{
    uint32_t start_code                   : 32;

    uint32_t length                       : 8;

    uint32_t id                           : 6;
    uint32_t protocol_min_version         : 5;
    uint32_t protocol_version             : 5;

    uint32_t CTUIA_flag                   : 2;
    uint32_t                              : 3;
    uint32_t NFP_flag                     : 1;
    uint32_t EB_flag                      : 2;

    uint32_t CMCT_update_index            : 4;
    uint32_t NIT_update_index             : 4;

    uint32_t SMCT_update_index            : 4;
    uint32_t CSCT_update_index            : 4;

    uint32_t ESG_update_index             : 4;
    uint32_t SSCT_update_index            : 4;

    uint32_t sub_frame_count              : 4;
    uint32_t ext_update_index             : 4;

    uint32_t *sub_frame_lengths;

    NextFrameParam *next_frame_params;
};

struct FreqPoint
{
    uint64_t index                        : 8;
    uint64_t central_freq                 : 32;
    uint64_t band_width                   : 4;
    uint64_t                              : 0;
};

struct AdjacentNet
{
    uint64_t id                           : 12;
    uint64_t level                        : 4;
    uint64_t freq_point_index             : 8;
    uint64_t central_freq                 : 32;
    uint64_t                              : 4;
    uint64_t band_width                   : 4;
};

struct TimeSlot
{
    uint8_t                               : 2;
    uint8_t index                         : 6;
};

struct MuxFrameParam
{
    uint32_t RS_rate                      : 2;
    uint32_t id                           : 6;

    uint32_t time_slot_count              : 6;
    uint32_t scramble_method              : 3;
    uint32_t reserved                     : 1;
    uint32_t modulation_method            : 2;
    uint32_t LDPC_rate                    : 2;
    uint32_t byte_interleave_mode         : 2;

    uint32_t sub_frame_count              : 4;
    uint32_t                              : 4;

    TimeSlot **time_slots;
    MuxSubFrameParam **sub_frame_params;
};

struct MuxSubFrameParam
{
    uint32_t                              : 4;
    uint32_t index                        : 4;

    uint32_t service_id                   : 16;
};

struct Service
{
    uint32_t id                           : 16;
    uint32_t freq_point_index             : 8;
};

struct CommonInfo
{
    uint32_t custom_id                    : 16;
    uint32_t ext_flag                     : 16;

    uint32_t                              : 4;
    uint32_t update_index                 : 4;
    uint32_t service_id                   : 16;
    uint32_t custom_name_length           : 8;

    uint8_t *custom_name;

    uint8_t trig_msg_length               : 8;
    uint8_t *trig_msg;
};

struct NIT
{
    uint32_t id                           : 8;

    uint32_t                              : 4;
    uint32_t update_index                 : 4;

    uint32_t MJD                          : 16;
    uint64_t BCD                          : 24;

    uint64_t country_code                 : 24;

    uint64_t net_id                       : 12;
    uint64_t net_level                    : 4;

    uint8_t net_name_length               : 8;

    uint8_t *net_name;

    uint8_t freq_point_index              : 8;

    uint32_t central_freq                 : 32;

    uint16_t other_freq_point_count       : 4;
    uint16_t band_width                   : 4;

    uint16_t                              : 4;
    uint16_t adjacent_net_count           : 4;

    FreqPoint **other_freq_points;

    AdjacentNet **adjacent_nets;
};

struct XMCT
{
    uint32_t id                           : 8;
    uint32_t freq_point_index             : 8;

    uint32_t mux_frame_count              : 6;
    uint32_t                              : 6;
    uint32_t update_index                 : 4;

    MuxFrameParam **mux_frame_params;
};

struct XSCT
{
    uint32_t id                           : 8;
    uint32_t seg_length                   : 16;
    uint32_t seg_index                    : 8;

    uint32_t seg_count                    : 8;
    uint32_t                              : 4;
    uint32_t update_index                 : 4;
    uint32_t service_count                : 16;

    Service **services;
};

struct ESGBDT
{
    uint8_t id                            : 8;
};

struct EADT
{
};

struct CIDT
{
    uint8_t id                            : 8;

    uint32_t                              : 12;
    uint32_t update_index                 : 4;
    union {
        uint32_t data_length              : 16;
        uint32_t ci_count                 : 16;
    };

    CommonInfo **comm_infos;
};

struct EB
{
    uint32_t id                           : 8;
    uint32_t msg_count                    : 4;
    uint32_t                              : 2;
    uint32_t index                        : 2;
    uint32_t data_length                  : 16;

    uint8_t *data;
};

struct EBDataSection
{
    uint16_t protocol_version             : 4;
    uint16_t protocol_min_version         : 4;
    uint16_t net_level                    : 4;
    uint16_t net_id                       : 12;
    uint16_t msg_id                       : 16;
    uint16_t curr_section_index           : 8;
    uint16_t last_section_index           : 8;
    uint16_t                              : 3;
    uint16_t data_length                  : 13;
    uint8_t *data;
};

struct EBMessage
{
    uint32_t trig_flag                    : 1;
    union {
        EBTrigParam *trig_param;
        EBMessageParam *msg_param;
    };
};

struct EBMessageParam
{

};

struct EBTrigParam
{

};

struct SectionSize
{
    uint32_t stream_count                 : 3;
    uint32_t length                       : 21;
};

struct DisplayParam
{
    uint16_t x                            : 6;
    uint16_t y                            : 6;
    uint16_t piority                      : 3;
    uint16_t                              : 1;
};

struct ResolutionParam
{
    uint32_t                              : 4;
    uint32_t width                        : 10;
    uint32_t height                       : 10;
};

struct VideoStreamParam
{
    uint32_t algorithm_type               : 3;
    uint32_t code_rate_flag               : 1;
    uint32_t display_flag                 : 1;
    uint32_t resolution_flag              : 1;
    uint32_t frame_freq_flag              : 1;
    uint32_t                              : 1;
    uint32_t code_rate                    : 16;
    uint32_t frame_freq                   : 4;
    uint32_t                              : 4;

    DisplayParam *display_param;
    ResolutionParam *resolution_param;
};

struct AudioStreamParam
{
    uint32_t                              : 1;
    uint32_t desc_flag                    : 1;
    uint32_t sample_rate_flag             : 1;
    uint32_t code_rate_flag               : 1;
    uint32_t algorithm_type               : 4;
    uint32_t                              : 2;
    uint32_t code_rate                    : 14;
    uint32_t sample_rate                  : 4;
    uint32_t                              : 4;

    uint32_t desc                         : 24;
};

struct VideoUnitParam
{
    uint16_t length                       : 16;

    uint16_t play_time_flag               : 1;
    uint16_t frame_end_flag               : 1;
    uint16_t stream_index                 : 3;
    uint16_t frame_type                   : 3;

    uint16_t play_time                    : 16;
};

struct AudioUnitParam
{
    uint16_t length                       : 16;

    uint16_t                              : 5;
    uint16_t stream_index                 : 3;

    uint16_t play_time                    : 16;
};

struct DataUnitParam
{
    uint16_t type                         : 8;
    uint16_t length                       : 16;
};

struct MuxSubFrame
{
    MuxSubFrameHeader *header;
    VideoSection *video_section;
    AudioSection *audio_section;
    DataSection *data_section;
};

struct MuxSubFrameHeader
{
    uint32_t length                       : 8;

    uint32_t pkg_mode_flag                : 1;
    uint32_t encryption_flag              : 2;
    uint32_t ext_flag                     : 1;
    uint32_t data_section_flag            : 1;
    uint32_t audio_section_flag           : 1;
    uint32_t video_section_flag           : 1;
    uint32_t start_play_time_flag         : 1;

    uint32_t start_play_time              : 32;

    SectionSize *video_section_size;
    SectionSize *audio_section_size;
    SectionSize *data_section_size;

    VideoStreamParam **video_stream_params;
    AudioStreamParam **audio_stream_params;
};

struct VideoSection
{
    uint32_t                              : 4;
    uint32_t unit_count                   : 12;

    VideoUnitParam **unit_params;
    uint8_t *data;
};

struct AudioSection
{
    uint8_t unit_count                    : 8;

    AudioUnitParam **unit_params;
    uint8_t *data;
};

struct DataSection
{
    uint8_t unit_count                    : 8;

    DataUnitParam **unit_params;
    uint8_t *data;
};

#ifdef __cplusplus
extern "C" {
#endif

extern NextFrameParam* next_frame_param_new(void);
extern void next_frame_param_free(NextFrameParam **nfp);

extern MuxFrameHeader* mux_frame_header_new(void);
extern void mux_frame_header_free(MuxFrameHeader **mfh);

extern FreqPoint* freq_point_new(void);
extern FreqPoint* freq_point_clone(FreqPoint **clone, FreqPoint *fp);
extern void freq_point_free(FreqPoint **fp);

extern AdjacentNet* adjacent_net_new(void);
extern AdjacentNet* adjacent_net_clone(AdjacentNet **clone, AdjacentNet *an);
extern void adjacent_net_free(AdjacentNet **an);

extern MuxFrameParam* mux_frame_param_new(void);
extern MuxFrameParam* mux_frame_param_clone(MuxFrameParam **clone, MuxFrameParam *mfp);
extern void mux_frame_param_free(MuxFrameParam **mfp);

extern TimeSlot* time_slot_new(void);
extern TimeSlot* time_slot_clone(TimeSlot **clone, TimeSlot *ts);
extern void time_slot_free(TimeSlot **ts);

extern MuxSubFrameParam* mux_sub_frame_param_new(void);
extern MuxSubFrameParam* mux_sub_frame_param_clone(MuxSubFrameParam **clone, MuxSubFrameParam *msfp);
extern void mux_sub_frame_param_free(MuxSubFrameParam **msfp);

extern Service* service_new(void);
extern Service* service_clone(Service **clone, Service *s);
extern void service_free(Service **s);

extern NIT* nit_new(void);
extern NIT* nit_clone(NIT **clone, NIT *nit);
extern void nit_free(NIT **nit);

extern XMCT* xmct_new(void);
extern XMCT* xmct_clone(XMCT **clone, XMCT *xmct);
extern void xmct_free(XMCT **xmct);

extern XSCT* xsct_new(void);
extern XSCT* xsct_clone(XSCT **clone, XSCT *xsct);
extern void xsct_free(XSCT **xsct);

extern ESGBDT* esgbdt_new(void);
extern ESGBDT* esgbdt_clone(ESGBDT **clone, ESGBDT *esgbdt);
extern void esgbdt_free(ESGBDT **esgbdt);

extern EB* eb_new(void);
extern void eb_free(EB **eb);

extern EADT* eadt_new(void);
extern EADT* eadt_clone(EADT **clone, EADT *eadt);
extern void eadt_free(EADT **eadt);

extern CIDT* cidt_new(void);
extern CIDT* cidt_clone(CIDT **clone, CIDT *cidt);
extern void cidt_free(CIDT **ci);

extern CommonInfo* common_info_new(void);
extern CommonInfo* common_info_clone(CommonInfo **clone, CommonInfo *ci);
extern void common_info_free(CommonInfo **ci);

extern EBDataSection* eb_data_section_new(void);
extern void eb_data_section_free(EBDataSection **ebds);

extern EBMessage* eb_message_new(void);
extern void eb_message_free(EBMessage **ebmsg);

extern EBTrigParam* eb_trig_param_new(void);
extern void eb_trig_param_free(EBTrigParam **ebtp);

extern EBMessageParam* eb_message_param_new(void);
extern void eb_message_param_free(EBMessageParam **ebmp);

extern MuxSubFrame* mux_sub_frame_new(void);
extern void mux_sub_frame_free(MuxSubFrame **msf);

extern MuxSubFrameHeader* mux_sub_frame_header_new(void);
extern void mux_sub_frame_header_free(MuxSubFrameHeader **msfh);

extern VideoSection* video_section_new(void);
extern void video_section_free(VideoSection **vs);

extern AudioSection* audio_section_new(void);
extern void audio_section_free(AudioSection **as);

extern DataSection* data_section_new(void);
extern void data_section_free(DataSection **ds);

extern SectionSize* section_size_new(void);
extern void section_size_free(SectionSize **ss);

extern DisplayParam* display_param_new(void);
extern void display_param_free(DisplayParam **dp);

extern ResolutionParam* resolution_param_new(void);
extern void resolution_param_free(ResolutionParam **rp);

extern VideoStreamParam* video_stream_param_new(void);
extern void video_stream_param_free(VideoStreamParam **vsp);

extern AudioStreamParam* audio_stream_param_new(void);
extern void audio_stream_param_free(AudioStreamParam **asp);

extern VideoUnitParam* video_unit_param_new(void);
extern void video_unit_param_free(VideoUnitParam **vup);

extern AudioUnitParam* audio_unit_param_new(void);
extern void audio_unit_param_free(AudioUnitParam **aup);

extern DataUnitParam* data_unit_param_new(void);
extern void data_unit_param_free(DataUnitParam **dup);


#ifdef __cplusplus
}
#endif

#endif /* __MFS_TYPES_H__ */
