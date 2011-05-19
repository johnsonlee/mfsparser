#include <mfstypes.h>

#ifndef __MFS_PARSER_H__
#define __MFS_PARSER_H__

typedef struct MFSParserVisitor MFSParserVisitor;
typedef enum MFSParserState MFSParserState;

enum MFSParserState
{
    STATE_NONE = 0
};

struct MFSParserVisitor
{
    void* (*visit_mux_frame_header)(MuxFrameHeader*, void*);
    void* (*visit_NIT)(NIT*, void*);
    void* (*visit_XMCT)(XMCT*, void*);
    void* (*visit_XSCT)(XSCT*, void*);
    void* (*visit_ESGBDT)(ESGBDT*, void*);
    void* (*visit_EB)(EB*, void*);
    void* (*visit_mux_sub_frame_header)(MuxSubFrameHeader*, void*);
    void* (*visit_video_section)(VideoSection*, void*);
    void* (*visit_audio_section)(AudioSection*, void*);
    void* (*visit_data_section)(DataSection*, void*);
};

#ifdef __cplusplus
extern "C" {
#endif

extern void parse_mux_frame(ByteBuffer *buf, MFSParserVisitor *visitor);

#ifdef __cplusplus
}
#endif

#endif /* __MFS_PARSER_H__ */
