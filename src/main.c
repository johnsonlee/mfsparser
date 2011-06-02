#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <log.h>
#include <mfsparser.h>

#define DAT_SECTION_PATTERN "%s/%02X-%d.dat"
#define DAT_UNIT_PATTERN    "%s/%02X-%02X-%02X-%d.dat" // MFID-MSFINDEX-DUTYPE-index

typedef struct MFSParserOption MFSParserOption;

struct MFSParserOption
{
    uint32_t with_nit       : 1;
    uint32_t with_cmct      : 1;
    uint32_t with_smct      : 1;
    uint32_t with_csct      : 1;
    uint32_t with_ssct      : 1;
    uint32_t with_esgbdt    : 1;
    uint32_t with_eb        : 1;
    uint32_t with_eadt      : 1;

    uint32_t with_cidt      : 1;
    uint32_t with_video     : 1;
    uint32_t with_audio     : 1;
    uint32_t with_esg       : 1;
    uint32_t with_prg       : 1;
    uint32_t with_xpe       : 1;
    uint32_t with_xpe_fec   : 1;
    uint32_t                : 1;

    uint32_t static_index   : 1;
    uint32_t verbose        : 1;

    size_t buffer_size;
    char *file;
    char *prefix;
};

static NIT *gs_nit;
static CMCT *gs_cmct;
static SMCT *gs_smct;
static CSCT *gs_csct;
static SSCT *gs_ssct;
static ESGBDT *gs_esgbdt;

static MFSParserOption optargs;
static const char *optstr = "af:hv?";
static const struct option opts[] = {
    { "with-all",       no_argument,        NULL, 'a' },
    { "with-nit",       no_argument,        NULL, 0 },
    { "with-cmct",      no_argument,        NULL, 0 },
    { "with-smct",      no_argument,        NULL, 0 },
    { "with-csct",      no_argument,        NULL, 0 },
    { "with-ssct",      no_argument,        NULL, 0 },
    { "with-esgbdt",    no_argument,        NULL, 0 },
    { "with-eb",        no_argument,        NULL, 0 },
    { "with-eadt",      no_argument,        NULL, 0 },
    { "with-cidt",      no_argument,        NULL, 0 },
    { "with-data",      no_argument,        NULL, 0 },
    { "with-video",     no_argument,        NULL, 0 },
    { "with-audio",     no_argument,        NULL, 0 },
    { "with-esg",       no_argument,        NULL, 0 },
    { "with-program",   no_argument,        NULL, 0 },
    { "with-xpe",       no_argument,        NULL, 0 },
    { "with-xpe-fec",   no_argument,        NULL, 0 },
    { "static-index",   no_argument,        NULL, 0 },
    { "buffer-size",    required_argument,  NULL, 0 },
    { "prefix",         required_argument,  NULL, 0 },
    { "suffix",         required_argument,  NULL, 0 },
    { "version",        no_argument,        NULL, 0 },
    { "help",           no_argument,        NULL, 'h' },
    { "verbose",        no_argument,        NULL, 'v' },
    { "file",           required_argument,  NULL, 'f' },
    { NULL,             no_argument,        NULL, 0 },
};

INLINE int
get_service_id(CMCT *cmct, MFSParser *parser)
{
    register int i, j;

    if (NULL == cmct)
        goto __ERROR__;

    MuxFrameHeader *mfh;
    MuxFrameParam *mfp;
    MuxSubFrameParam *msfp;

    mfh = mfsparser_get_mux_frame_header(parser);
    int sub_frm_index = mfsparser_get_current_sub_frame_index(parser);
    for (i = 0; i < cmct->mux_frame_count; i++) {
        mfp = cmct->mux_frame_params[i];

        for (j = 0; j < mfp->sub_frame_count; j++) {
            msfp = mfp->sub_frame_params[j];

            if (mfp->id == mfh->id && sub_frm_index == msfp->index) {
                return msfp->service_id;
            }
        }
    }

__ERROR__:
    return -2;
}

INLINE void
visit_NIT(NIT *nit, MFSParser *parser)
{
    if (gs_nit == NULL || gs_nit->update_index != nit->update_index) {
        nit_clone(&gs_nit, nit);
    }

    if (!optargs.with_nit)
        return;
}

INLINE void
visit_CMCT(CMCT *cmct, MFSParser *parser)
{
#if 0
    if (gs_cmct == NULL || gs_cmct->update_index != cmct->update_index) {
        xmct_clone(&gs_cmct, cmct);
    }
#endif

    if (!optargs.with_cmct)
        return;
}

INLINE void
visit_SMCT(SMCT *smct, MFSParser *parser)
{
#if 0
    if (gs_smct == NULL || gs_smct->update_index != smct->update_index) {
        xmct_clone(&gs_smct, smct);
    }
#endif

    if (!optargs.with_smct)
        return;
}

INLINE void
visit_CSCT(CSCT *csct, MFSParser *parser)
{
#if 0
    if (gs_csct == NULL || gs_csct->update_index != csct->update_index) {
        xsct_clone(&gs_csct, csct);
    }
#endif

    if (!optargs.with_csct)
        return;
}

INLINE void
visit_SSCT(SSCT *ssct, MFSParser *parser)
{
#if 0
    if (gs_ssct == NULL || gs_ssct->update_index != ssct->update_index) {
        xsct_clone(&gs_ssct, ssct);
    }
#endif

    if (!optargs.with_ssct)
        return;
}

INLINE void
visit_ESGBDT(ESGBDT *esgbdt, MFSParser *parser)
{
#if 0
    if (gs_esgbdt == NULL/* || gs_esgbdt->update_index != esgbdt->update_index*/) {
        esgbdt_clone(&gs_esgbdt, esgbdt);
    }
#endif

    if (!optargs.with_esgbdt)
        return;
}

INLINE void
visit_EB(EB *eb, MFSParser *parser)
{
    if (!optargs.with_eb)
        return;
}

INLINE void
visit_EADT(EADT *eadt, MFSParser *parser)
{
    if (!optargs.with_eadt)
        return;
}

INLINE void
visit_CIDT(CIDT *cidt, MFSParser *parser)
{
    if (!optargs.with_cidt)
        return;
}

INLINE void
visit_video_section(MuxSubFrameHeader *msfh, VideoSection *vs, MFSParser *parser)
{
    if (!optargs.with_video)
        return;

    char path[255];
    register int i, fd, mf_id, msf_index, length;

    mf_id = mfsparser_get_mux_frame_header(parser)->id;
    msf_index = mfsparser_get_current_sub_frame_index(parser);
    sprintf(path, DAT_SECTION_PATTERN, optargs.prefix, mf_id, msf_index);
    if (-1 == (fd = open(path, O_CREAT | O_WRONLY)))
        return;

    for (i = length = 0; i < vs->unit_count; i++) {
        length += vs->unit_params[i]->length;
    }
    write(fd, vs->data, length);
    debug("write %d bytes to `%s'\n", length, path);
    close(fd);
}

INLINE void
visit_audio_section(MuxSubFrameHeader *msfh, AudioSection *as, MFSParser *parser)
{
    if (!optargs.with_audio)
        return;

    char path[255];
    register int i, fd, mf_id, msf_index, length;

    mf_id = mfsparser_get_mux_frame_header(parser)->id;
    msf_index = mfsparser_get_current_sub_frame_index(parser);
    sprintf(path, DAT_SECTION_PATTERN, optargs.prefix, mf_id, msf_index);
    if (-1 == (fd = open(path, O_CREAT | O_WRONLY)))
        return;

    for (i = length = 0; i < as->unit_count; i++) {
        length += as->unit_params[i]->length;
    }
    write(fd, as->data, length);
    debug("write %d bytes to `%s'\n", length, path);
    close(fd);
}

INLINE void visit_data_section(MuxSubFrameHeader *msfh, DataSection *ds, MFSParser *parser)
{
    static int static_index = 0;

    char path[255];
    int i, n, offset, fd, type, mf_id, msf_index;

    mf_id = mfsparser_get_mux_frame_header(parser)->id;
    msf_index = mfsparser_get_current_sub_frame_index(parser);
    for (i = offset = 0; i < ds->unit_count; i++) {
        type = ds->unit_params[i]->type;
        n = optargs.static_index ? static_index : i;
        sprintf(path, DAT_UNIT_PATTERN, optargs.prefix, mf_id, msf_index, type, n);

        switch (type) {
            case DATA_UNIT_TYPE_ESG:
                if (!optargs.with_esg)
                    goto __NEXT__;
                break;
            case DATA_UNIT_TYPE_PROGRAM:
                if (!optargs.with_prg)
                    goto __NEXT__;
                break;
            case DATA_UNIT_TYPE_XPE:
                if (!optargs.with_xpe)
                    goto __NEXT__;
                break;
            case DATA_UNIT_TYPE_XPE_FEC:
                if (!optargs.with_xpe_fec)
                    goto __NEXT__;
                break;
            default:
                goto __NEXT__;
        }

        if (-1 == (fd = open(path, O_CREAT | O_WRONLY)))
            goto __NEXT__;

        n = write(fd, ds->data + offset, ds->unit_params[i]->length);
        debug("write %d bytes to `%s'\n", n, path);
        close(fd);

        static_index++;

__NEXT__:
        offset += ds->unit_params[i]->length;
    }
}

static MFSParserVisitor visitor = {
    visit_NIT,
    visit_CMCT,
    visit_SMCT,
    visit_CSCT,
    visit_SSCT,
    visit_ESGBDT,
    visit_EB,
    visit_EADT,
    visit_CIDT,
    visit_video_section,
    visit_audio_section,
    visit_data_section
};

INLINE void print_usage(int argc, char** argv)
{
    info("Usage: %s [OPTION] [FILE]...\n", argv[0]);
    info("解析复用帧(CMMB标准)\n");
    info("\n");
    info("  -h, --help          打印帮助信息并退出\n");
    info("  -f, --file          输入文件\n");
    info("  -v, --verbose       打印详细信息\n");
    info("  -a, --with-all      输出解析出的所有数据\n");
    info("      --with-nit      输出网络信息表\n");
    info("      --with-cmct     输出持续时间业务复用配置表\n");
    info("      --with-smct     输出短时间业务复用配置表\n");
    info("      --with-csct     输出持续时间业务配置表\n");
    info("      --with-ssct     输出短时间业务配置表\n");
    info("      --with-esgbdt   输出ESG基本描述表\n");
    info("      --with-eb       输出紧急广播\n");
    info("      --with-eadt     输出加密授权描述表\n");
    info("      --with-cidt     输出公共信息描述表\n");
    info("      --with-data     输出音、视频及数据段\n");
    info("      --with-video    输出视频段\n");
    info("      --with-audio    输出音频段\n");
    info("      --with-esg      输出ESG数据单元\n");
    info("      --with-prg      输出节目提示信息数据单元\n");
    info("      --with-xpe      输出XPE包数据单元\n");
    info("      --with-xpe-fec  输出XPE-FEC包数据单元\n");
    info("      --static-index  使用静态索引号命名输出的数据文件\n");
    info("      --buffer-size   指定解析器使用的缓冲区大小\n");
    info("      --version       打印版本信息并退出\n");
}

INLINE void print_version(int argc, char **argv)
{
    info("mfsparser (CMMB Middleware Tools) 1.0\n");
    info("Copyright (C) 2011 Kortide, Inc.\n");
    info("\n");
}

int main(int argc, char **argv)
{
    char c;
    int i;

    ((uint32_t*) &optargs)[0] = 0;
    optargs.prefix = getenv("HOME");
    optargs.buffer_size = 81920;

    for (; EOF != (c = getopt_long(argc, argv, optstr, opts, &i));) {
        switch (c) {
        case 'a':
        __WITH_ALL__:
            ((uint16_t*) &optargs)[0] = 0xFFFF;
            break;
        case 'f':
            optargs.file = optarg;
            break;
        case 'h':
        case '?':
            print_usage(argc, argv);
            exit(0);
            break;
        case 'v':
            optargs.verbose = 1;
            break;
        case 0:
            if (!strcmp("with-all", opts[i].name)) {
                goto __WITH_ALL__;
            } else if (!strcmp("with-nit", opts[i].name)) {
                optargs.with_nit = 1;
            } else if (!strcmp("with-cmct", opts[i].name)) {
                optargs.with_cmct = 1;
            } else if (!strcmp("with-smct", opts[i].name)) {
                optargs.with_smct = 1;
            } else if (!strcmp("with-csct", opts[i].name)) {
                optargs.with_csct = 1;
            } else if (!strcmp("with-ssct", opts[i].name)) {
                optargs.with_ssct = 1;
            } else if (!strcmp("with-esgbdt", opts[i].name)) {
                optargs.with_esgbdt = 1;
            } else if (!strcmp("with-eb", opts[i].name)) {
                optargs.with_eb = 1;
            } else if (!strcmp("with-eadt", opts[i].name)) {
                optargs.with_eadt = 1;
            } else if (!strcmp("with-cidt", opts[i].name)) {
                optargs.with_cidt = 1;
            } else if (!strcmp("with-data", opts[i].name)) {
                ((unsigned char*) &optargs)[1] = 0xFF;
            } else if (!strcmp("with-video", opts[i].name)) {
                optargs.with_video = 1;
            } else if (!strcmp("with-audio", opts[i].name)) {
                optargs.with_audio = 1;
            } else if (!strcmp("with-esg", opts[i].name)) {
                optargs.with_esg = 1;
            } else if (!strcmp("with-prg", opts[i].name)) {
                optargs.with_prg = 1;
            } else if (!strcmp("with-xpe", opts[i].name)) {
                optargs.with_xpe = 1;
            } else if (!strcmp("with-xpe-fec", opts[i].name)) {
                optargs.with_xpe_fec = 1;
            } else if (!strcmp("static-index", opts[i].name)) {
                optargs.static_index = 1;
            } else if (!strcmp("buffer-size", opts[i].name)) {
                optargs.buffer_size = atol(optarg);
                if (optargs.buffer_size <= 0) {
                    error("valid buffer size!\n");
                    exit(0);
                }
            } else if (!strcmp("prefix", opts[i].name)) {
                optargs.prefix = optarg;
            } else if (!strcmp("file", opts[i].name)) {
                optargs.file = optarg;
            } else if (!strcmp("version", opts[i].name)) {
                print_version(argc, argv);
                exit(0);
            }
            break;
        default:
            break;
        }
    }

    if (!optargs.file) {
        debug("%d\n", i);
        error("%s: missing source file operand after `%s'\n", argv[0], argv[argc - 1]);
        error("Try `%s --help' for more information.\n", argv[0]);
        exit(0);
    }

    unsigned char *buf = malloc(sizeof(unsigned char*) * optargs.buffer_size);
    int fd_dat = open(optargs.file, O_RDONLY);

    if (-1 == fd_dat) {
        error("open file %s failed!\n", optargs.file);
        exit(0);
    }

    ByteStream *stream = stream_new();
    MFSParser *parser = mfsparser_new();

    while (0 < read(fd_dat, buf, optargs.buffer_size)) {
        stream_trace(stream);
        stream_write(stream, buf, optargs.buffer_size);
        mfsparser_parse_mux_frame(parser, stream, &visitor);
    }

    close(fd_dat);

    mfsparser_free(&parser);
    stream_free(&stream);
    free(buf);

    return 0;
}
