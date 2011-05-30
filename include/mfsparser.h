#include <mfstypes.h>

#ifndef __MFS_PARSER_H__
#define __MFS_PARSER_H__

#ifndef BUF_SIZE
#define BUF_SIZE                80960
#endif

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
    void* (*visit_NIT)(NIT *nit, void *obj);
    void* (*visit_CMCT)(CMCT *cmct, void *obj);
    void* (*visit_SMCT)(SMCT *smct, void *obj);
    void* (*visit_CSCT)(CSCT *csct, void *obj);
    void* (*visit_SSCT)(SSCT *ssct, void *obj);
    void* (*visit_ESGBDT)(ESGBDT *esgbdt, void *obj);
    void* (*visit_EB)(EB *eb, void *obj);
    void* (*visit_video_section)(MuxSubFrameHeader *msfh, VideoSection *vs, void *obj);
    void* (*visit_audio_section)(MuxSubFrameHeader *msfh, AudioSection *as, void *obj);
    void* (*visit_data_section)(MuxSubFrameHeader *msfh, DataSection *ds, void *obj);
};

#ifdef __cplusplus
extern "C" {
#endif

INLINE int mfsparser_find_header(unsigned char *data, int length)
{
    register int i = 0;
    static const unsigned char start_code[] = { 0, 0, 0, 1 };

    for (i = 0; i < length - 4; i++) {
        if (!memcmp(data, start_code, 4)) {
            return i;
        }
    }

    return -1;
}

extern MFSParser* mfsparser_new(void);

extern MuxFrameHeader* mfsparser_parse_header(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor);

extern void mfsparser_parse(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor);

extern void mfsparser_free(MFSParser **parser);

#ifdef __cplusplus
}
#endif

#endif /* __MFS_PARSER_H__ */