#include <stddef.h>
#include <stdlib.h>
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

typedef struct FrameLength           FrameLength;           // multiplex frame length
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

struct FrameLength
{
    unsigned int length                   : 24;
};

struct NextFrameParam
{
    unsigned char header_length           : 8;
    unsigned int frame1_length            : 24;
    unsigned int frame1_header_length     : 8;
};

struct MuxFrameHeader
{
    unsigned int start_code               : 32;

    unsigned char length                  : 8;

    unsigned char protocol_version        : 5;
    unsigned char protocal_min_version    : 5;
    unsigned char id                      : 6;

    unsigned char EB_flag                 : 2;
    unsigned char NFP_flag                : 1;
    unsigned char CTUIA_flag              : 2;
    unsigned char                         : 3;

    unsigned char NIT_update_index        : 4;
    unsigned char CMCT_update_index       : 4;

    unsigned char CSCT_update_index       : 4;
    unsigned char SMCT_update_index       : 4;

    unsigned char SSCT_update_index       : 4;
    unsigned char ESG_update_index        : 4;

    unsigned char sub_frame_number        : 4;
    unsigned char                         : 0;

    FrameLength *sub_frame_lengths;

    NextFrameParam *next_frame_params;
};

struct FreqPoint
{
    unsigned char index                   : 8;

    unsigned int central_freq             : 32;

    unsigned char band_width              : 4;
    unsigned char                         : 0;
};

struct AdjacentNet
{
    unsigned char level                   : 4;
    unsigned int id                       : 12;

    unsigned char freq_point_index        : 8;

    unsigned int central_freq             : 32;

    unsigned char band_width              : 4;
    unsigned char                         : 0;
};

struct MuxFrameParam
{
    unsigned char id                      : 6;
    unsigned char RS_rate                 : 2;

    unsigned char byte_interleave_mode    : 2;
    unsigned char LDPC_rate               : 2;
    unsigned char modulation_method       : 2;
    unsigned char reserved                : 1;
    unsigned char scramble_method         : 3;
    unsigned char time_slot_number        : 6;

    TimeSlot *time_slots;

    unsigned char sub_frame_number        : 4;
    unsigned char                         : 0;

    MuxSubFrameParam *sub_frame_params;
};

struct MuxSubFrameParam
{
    unsigned char sub_frame_id            : 4;
    unsigned char                         : 0;

    unsigned int service_id               : 16;
};

struct Service
{
    unsigned int id                       : 16;
    unsigned char freq_point_index        : 8;
};

struct NIT
{
    unsigned char id                      : 8;

    unsigned char NIT_update_index        : 4;
    unsigned char                         : 0;

    unsigned int MJD                      : 16;
    unsigned int BCD                      : 24;

    unsigned int country_code             : 24;

    struct
    {
        unsigned int level                : 4;
        unsigned int id                   : 12;
        unsigned char name_length         : 8;
        unsigned char *name;
    } network;

    unsigned char freq_point_index        : 8;

    unsigned int central_freq             : 32;

    unsigned char band_width              : 4;
    unsigned char other_freq_point_number : 4;

    FreqPoint *other_freq_points;

    unsigned char adjacent_net_number     : 4;
    unsigned char                         : 0;

    AdjacentNet *adjacent_nets;
};

struct XMCT
{
    unsigned char id                      : 8;
    unsigned char freq_point_index        : 8;

    unsigned char update_index            : 4;
    unsigned char                         : 0;

    unsigned char mux_frame_number        : 6;
    unsigned char                         : 0;

    MuxFrameParam *mux_frame_params;
};

struct XSCT
{
    unsigned char id                      : 8;

    unsigned int seg_length               : 16;

    unsigned char seg_index               : 8;

    unsigned char seg_number              : 8;

    unsigned char update_index            : 4;
    unsigned char                         : 0;

    unsigned int service_number           : 16;

    Service *services;
};

struct EB
{
    unsigned char id                      : 8;

    unsigned char index                   : 2;
    unsigned char                         : 0;

    unsigned int data_length              : 16;

    unsigned char *data;
};

struct SectionSize
{
    unsigned int length                   : 21;
    unsigned int stream_number            : 3;
};

struct DisplayParam
{
    unsigned int x                        : 6;
    unsigned int y                        : 6;
    unsigned int piority                  : 3;
    unsigned int                          : 0;
};

struct ResolutionParam
{
    unsigned int width                    : 10;
    unsigned int height                   : 10;
    unsigned int                          : 0;
};

struct VideoStreamParam
{
    unsigned char algorithm_type          : 3;
    unsigned char code_rate_flag          : 1;
    unsigned char display_flag            : 1;
    unsigned char resolution_flag         : 1;
    unsigned char frame_freq_flag         : 1;
    unsigned char                         : 0;

    unsigned int code_rate                : 16;

    unsigned char frame_freq              : 4;
    unsigned char                         : 0;

    DisplayParam *display_param;
    ResolutionParam *resolution_param;
};

struct AudioStreamParam
{
    unsigned char algorithm_type          : 4;
    unsigned char code_rate_flag          : 1;
    unsigned char sample_rate_flag        : 1;
    unsigned char desc_flag               : 1;
    unsigned char                         : 0;

    unsigned int code_rate                : 14;
    unsigned int                          : 0;

    unsigned char sample_rate             : 4;
    unsigned char                         : 0;

    unsigned int desc                     : 24;
};

struct VideoUnitParam
{
    unsigned int length                   : 16;

    unsigned char frame_type              : 3;
    unsigned char stream_index            : 3;
    unsigned char frame_end_flag          : 1;
    unsigned char play_time_flag          : 1;

    unsigned int play_time                : 16;
};

struct AudioUnitParam
{
    unsigned int length                   : 16;

    unsigned char stream_index            : 3;
    unsigned char                         : 0;

    unsigned int play_time                : 16;
};

struct DataUnitParam
{
    unsigned char type                    : 8;

    unsigned int length                   : 16;
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
    unsigned char length                  : 8;

    unsigned char start_play_time_flag    : 1;
    unsigned char video_section_flag      : 1;
    unsigned char audio_section_flag      : 1;
    unsigned char data_section_flag       : 1;
    unsigned char ext_flag                : 1;
    unsigned char                         : 0;

    unsigned int start_play_time          : 32;

    SectionSize *video_section_size;
    SectionSize *audio_section_size;
    SectionSize *data_section_size;

    VideoStreamParam **video_stream_params;
    AudioStreamParam **audio_stream_params;
};

struct VideoSection
{
    unsigned int header_length            : 12;
    unsigned int                          : 4;

    VideoUnitParam **unit_params;
    ByteBuffer **units;
};

struct AudioSection
{
    unsigned char unit_number             : 8;

    AudioUnitParam **unit_params;
    ByteBuffer **units;
};

struct DataSection
{
    unsigned char unit_number             : 8;

    DataUnitParam **unit_params;
    ByteBuffer **units;
};

#endif /* __MFS_TYPES_H__ */
