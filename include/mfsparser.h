#include <mfstypes.h>
#include <bytestream.h>

#ifndef __MFS_PARSER_H__
#define __MFS_PARSER_H__

typedef enum MFSParserState     MFSParserState;     // 解析器状态
typedef struct MFSParser        MFSParser;          // MFS解析器
typedef struct MFSParserVisitor MFSParserVisitor;   // MFS解析器访问器

enum MFSParserState
{
    STATE_NONE = 0x00,  // 初始状态
    STATE_CIT,          // 控制信息表
    STATE_MSF,          // 复用子帧
};

struct MFSParserVisitor
{
    void (*visit_NIT)(NIT *nit, MFSParser *parser);
    void (*visit_CMCT)(CMCT *cmct, MFSParser *parser);
    void (*visit_SMCT)(SMCT *smct, MFSParser *parser);
    void (*visit_CSCT)(CSCT *csct, MFSParser *parser);
    void (*visit_SSCT)(SSCT *ssct, MFSParser *parser);
    void (*visit_ESGBDT)(ESGBDT *esgbdt, MFSParser *parser);
    void (*visit_EB)(EB *eb, MFSParser *parser);
    void (*visit_EADT)(EADT *eb, MFSParser *parser);
    void (*visit_CIDT)(CIDT *ci, MFSParser *parser);
    void (*visit_video_section)(MuxSubFrameHeader *msfh, VideoSection *vs, MFSParser *parser);
    void (*visit_audio_section)(MuxSubFrameHeader *msfh, AudioSection *as, MFSParser *parser);
    void (*visit_data_section)(MuxSubFrameHeader *msfh, DataSection *ds, MFSParser *parser);
};

#ifdef __cplusplus
extern "C" {
#endif

extern MFSParser* mfsparser_new(void);

extern void mfsparser_set_state(MFSParser *parser, MFSParserState state);

extern MFSParserState mfsparser_get_state(MFSParser *parser);

extern MuxFrameHeader* mfsparser_get_mux_frame_header(MFSParser *parser);

extern size_t mfsparser_get_current_frame_size(MFSParser *parser);

extern int mfsparser_get_current_sub_frame_index(MFSParser *parser);

extern size_t mfsparser_get_current_sub_frame_size(MFSParser *parser);

extern MuxFrameHeader* mfsparser_parse_mux_frame_header(MFSParser *parser, ByteStream *stream);

extern void mfsparser_parse_mux_frame(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor);

extern void mfsparser_free(MFSParser **parser);

#ifdef __cplusplus
}
#endif

#endif /* __MFS_PARSER_H__ */
