#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <bytebuffer.h>

#ifndef __MFS_TYPES_H__
#define __MFS_TYPES_H__

typedef enum RSCodeRate              RSCodeRate;
typedef enum ByteInterleavedMode     ByteInterleavedMode;
typedef enum LDPCCodeRate            LDPCCodeRate;
typedef enum ModulationMethod        ModulationMethod;
typedef enum ScrambleMethod          ScrambleMethod;
typedef enum FrameFrequency          FrameFrequency;
typedef enum SampleRate              SampleRate;
typedef enum FrameType               FrameType;
typedef enum DataUnitType            DataUnitType;

typedef struct NextFrameParam        NextFrameParam;        // next multiplex frame parameter
typedef struct MuxFrameHeader        MuxFrameHeader;        // multiplex frame header
typedef struct FreqPoint             FreqPoint;             // freqency point
typedef struct AdjacentNet           AdjacentNet;           // adjacent network
typedef struct MuxFrameParam         MuxFrameParam;         // multiplex frame parameter
typedef struct TimeSlot              TimeSlot;              // time slot
typedef struct MuxSubFrameParam      MuxSubFrameParam;      // multiplex sub frame parameter
typedef struct Service               Service;
typedef struct NIT                   NIT;                   // network info table
typedef struct XMCT                  XMCT, CMCT, SMCT;      // continual/short time service multiplex configuration table
typedef struct XSCT                  XSCT, CSCT, SSCT;      // continual/short time service configuration table
typedef struct ESGBDT                ESGBDT;                // electronic service guide basic description table
typedef struct EB                    EB;                    // emergency broadcasting

typedef struct MuxSubFrame           MuxSubFrame;           // multiplex sub frame
typedef struct MuxSubFrameHeader     MuxSubFrameHeader;     // multiplex sub frame header
typedef struct VideoSection          VideoSection;
typedef struct AudioSection          AudioSection;
typedef struct DataSection           DataSection;
typedef struct SectionSize           SectionSize;
typedef struct DisplayParam          DisplayParam;
typedef struct ResolutionParam       ResolutionParam;
typedef struct VideoStreamParam      VideoStreamParam;
typedef struct AudioStreamParam      AudioStreamParam;
typedef struct VideoUnitParam        VideoUnitParam;
typedef struct AudioUnitParam        AudioUnitParam;
typedef struct DataUnitParam         DataUnitParam;

enum RSCodeRate
{
    RS_CODE_RATE_240_240,
    RS_CODE_RATE_240_224,
    RS_CODE_RATE_240_192,
    RS_CODE_RATE_240_176
};

enum ByteInterleavedMode
{
    BYTE_INTERLEAVE_MODE_1 = 0x01,
    BYTE_INTERLEAVE_MODE_2,
    BYTE_INTERLEAVE_MODE_3,
};

enum LDPCCodeRate
{
    LDPC_CODE_RATE_1_2,
    LDPC_CODE_RATE_3_4
};

enum ModulationMethod
{
    MODULATION_METHOD_BPSK,
    MODULATION_METHOD_QPSK,
    MODULATION_METHOD_16QAM
};

enum ScrambleMethod
{
    SCRAMBLE_METHOD_0,
    SCRAMBLE_METHOD_1,
    SCRAMBLE_METHOD_2,
    SCRAMBLE_METHOD_3,
    SCRAMBLE_METHOD_4,
    SCRAMBLE_METHOD_5,
    SCRAMBLE_METHOD_6,
    SCRAMBLE_METHOD_7,
};

enum FrameFrequency
{
    FRAME_FREQUENCY_25HZ,
    FRAME_FREQUENCY_30HZ,
    FRAME_FREQUENCY_12_5HZ,
    FRAME_FREQUENCY_15HZ,
};

enum SampleRate
{
    SAMPLE_RATE_8HZ,
    SAMPLE_RATE_12HZ,
    SAMPLE_RATE_16HZ,
    SAMPLE_RATE_22_05HZ,
    SAMPLE_RATE_24HZ,
    SAMPLE_RATE_32HZ,
    SAMPLE_RATE_44_1HZ,
    SAMPLE_RATE_48HZ,
    SAMPLE_RATE_96HZ
};

enum FrameType
{
    FRAME_TYPE_I,
    FRAME_TYPE_P,
    FRAME_TYPE_B,
};

enum DataUnitType
{
    DATA_UNIT_TYPE_ESG
};

struct NextFrameParam
{
    uint32_t header_length                 : 8;
    uint32_t frame1_length                 : 24;
    uint8_t frame1_header_length           : 8;
};

struct MuxFrameHeader
{
    uint32_t start_code               : 32;

    uint32_t length                   : 8;
    uint32_t protocol_version         : 5;
    uint32_t protocal_min_version     : 5;
    uint32_t id                       : 6;
    uint32_t EB_flag                  : 2;
    uint32_t NFP_flag                 : 1;
    uint32_t CTUIA_flag               : 2;
    uint32_t                          : 3;

    uint32_t NIT_update_index         : 4;
    uint32_t CMCT_update_index        : 4;
    uint32_t CSCT_update_index        : 4;
    uint32_t SMCT_update_index        : 4;
    uint32_t SSCT_update_index        : 4;
    uint32_t ESG_update_index         : 4;
    uint32_t                          : 4;
    uint32_t sub_frame_number         : 4;

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
    uint64_t level                        : 4;
    uint64_t id                           : 12;
    uint64_t freq_point_index             : 8;
    uint64_t central_freq                 : 32;
    uint64_t band_width                   : 4;
    uint64_t                              : 4;
};

struct TimeSlot
{
    uint8_t index                         : 6;
    uint8_t                               : 2;
};

struct MuxFrameParam
{
    uint32_t id                           : 6;
    uint32_t RS_rate                      : 2;
    uint32_t byte_interleave_mode         : 2;
    uint32_t LDPC_rate                    : 2;
    uint32_t modulation_method            : 2;
    uint32_t reserved                     : 1;
    uint32_t scramble_method              : 3;
    uint32_t time_slot_number             : 6;
    uint32_t sub_frame_number             : 4;
    uint32_t                              : 4;

    TimeSlot **time_slots;
    MuxSubFrameParam **sub_frame_params;
};

struct MuxSubFrameParam
{
    uint32_t sub_frame_id                 : 4;
    uint32_t                              : 4;
    uint32_t service_id                   : 16;
};

struct Service
{
    uint32_t id                           : 16;
    uint32_t freq_point_index             : 8;
};

struct NIT
{
    uint32_t id                           : 8;
    uint32_t NIT_update_index             : 4;
    uint32_t                              : 4;
    uint32_t MJD                          : 16;

    uint64_t BCD                          : 24;
    uint64_t country_code                 : 24;
    uint64_t net_level                    : 4;
    uint64_t net_id                       : 12;

    uint8_t net_name_length               : 8;

    uint8_t *net_name;

    uint8_t freq_point_index              : 8;

    uint32_t central_freq                 : 32;

    uint16_t band_width                   : 4;
    uint16_t other_freq_point_number      : 4;
    uint16_t adjacent_net_number          : 4;
    uint16_t                              : 4;

    FreqPoint **other_freq_points;

    AdjacentNet **adjacent_nets;
};

struct XMCT
{
    uint32_t id                           : 8;
    uint32_t freq_point_index             : 8;
    uint32_t update_index                 : 4;
    uint32_t                              : 6;
    uint32_t mux_frame_number             : 6;

    MuxFrameParam **mux_frame_params;
};

struct XSCT
{
    uint32_t id                           : 8;
    uint32_t seg_length                   : 16;
    uint32_t seg_index                    : 8;

    uint32_t seg_number                   : 8;
    uint32_t update_index                 : 4;
    uint32_t                              : 4;
    uint32_t service_number               : 16;

    Service **services;
};

struct ESGBDT
{
};

struct EB
{
    uint32_t id                           : 8;
    uint32_t                              : 6;
    uint32_t index                        : 2;
    uint32_t data_length                  : 16;

    uint8_t *data;
};

struct SectionSize
{
    uint32_t length                       : 21;
    uint32_t stream_number                : 3;
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
    uint32_t algorithm_type               : 4;
    uint32_t code_rate_flag               : 1;
    uint32_t sample_rate_flag             : 1;
    uint32_t desc_flag                    : 1;
    uint32_t                              : 1;
    uint32_t code_rate                    : 14;
    uint32_t                              : 2;
    uint32_t sample_rate                  : 4;
    uint32_t                              : 4;

    uint32_t desc                         : 24;
};

struct VideoUnitParam
{
    uint16_t length                       : 16;

    uint16_t frame_type                   : 3;
    uint16_t stream_index                 : 3;
    uint16_t frame_end_flag               : 1;
    uint16_t play_time_flag               : 1;
    uint16_t                              : 8;

    uint16_t play_time                    : 16;
};

struct AudioUnitParam
{
    uint16_t length                       : 16;

    uint16_t stream_index                 : 3;
    uint16_t                              : 0;

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
    uint32_t start_play_time_flag         : 1;
    uint32_t video_section_flag           : 1;
    uint32_t audio_section_flag           : 1;
    uint32_t data_section_flag            : 1;
    uint32_t ext_flag                     : 1;
    uint32_t                              : 0;

    uint32_t start_play_time              : 32;

    SectionSize *video_section_size;
    SectionSize *audio_section_size;
    SectionSize *data_section_size;

    VideoStreamParam **video_stream_params;
    AudioStreamParam **audio_stream_params;
};

struct VideoSection
{
    uint32_t header_length                : 12;
    uint32_t                              : 4;

    VideoUnitParam **unit_params;
    ByteBuffer **units;
};

struct AudioSection
{
    uint8_t unit_number                   : 8;

    AudioUnitParam **unit_params;
    ByteBuffer **units;
};

struct DataSection
{
    uint8_t unit_number                   : 8;

    DataUnitParam **unit_params;
    ByteBuffer **units;
};

#define construct(type) malloc(sizeof(type))

#define nconstruct(type, n) malloc(sizeof(type) * (n));

#define finalize(object) free(object); object = NULL

#ifdef __cplusplus
extern "C" {
#endif

extern void next_frame_param_free(NextFrameParam**);
extern void mux_frame_header_free(MuxFrameHeader**);
extern void freq_point_free(FreqPoint**);
extern void adjacent_net_free(AdjacentNet**);
extern void time_slot_free(TimeSlot**);
extern void mux_frame_param_free(MuxFrameParam**);
extern void mux_sub_frame_param_free(MuxSubFrameParam**);
extern void service_free(Service**);
extern void nit_free(NIT**);
extern void xmct_free(XMCT**);
extern void xsct_free(XSCT**);
extern void esgbdt_free(ESGBDT**);
extern void eb_free(EB**);
extern void section_size_free(SectionSize**);
extern void display_param_free(DisplayParam**);
extern void resolution_param_free(ResolutionParam**);
extern void video_stream_param_free(VideoStreamParam**);
extern void audio_stream_param_free(AudioStreamParam**);
extern void video_unit_param_free(VideoUnitParam**);
extern void audio_unit_param_free(AudioUnitParam**);
extern void data_unit_param_free(DataUnitParam**);
extern void mux_sub_frame_free(MuxSubFrame**);
extern void mux_sub_frame_header_free(MuxSubFrameHeader**);
extern void video_section_free(VideoSection**);
extern void audio_section_free(AudioSection**);
extern void data_section_free(DataSection**);

#ifdef __cplusplus
}
#endif

#endif /* __MFS_TYPES_H__ */
