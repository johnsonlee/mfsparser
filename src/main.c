#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <log.h>
#include <mfsparser.h>

#define BUF_SIZE            80960

#define DAT_FILE_DIR        ".cmmb"
#define DAT_FILE_ESGBDT     DAT_FILE_DIR"/ESGBDT.dat"
#define DAT_FILE_MFS_TS0    DAT_FILE_DIR"/TS0.dat"
#define DAT_FILE_MFS_254    DAT_FILE_DIR"/MFS-254.dat"
#define DAT_FILE_ESG        DAT_FILE_DIR"/ESG.dat"

INLINE void* visit_NIT(NIT *nit, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_CMCT(CMCT *cmct, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_SMCT(SMCT *smct, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_CSCT(CSCT *csct, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_SSCT(SSCT *ssct, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_ESGBDT(ESGBDT *esgbdt, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_EB(EB *eb, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_video_section(MuxSubFrameHeader *msfh, VideoSection *vs, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_audio_section(MuxSubFrameHeader *msfh, AudioSection *as, void *obj)
{
    debug("\n");
    return obj;
}

INLINE void* visit_data_section(MuxSubFrameHeader *msfh, DataSection *ds, void *obj)
{
    debug("\n");

#if 1
    register int i, n, offset;
    char path[255];

    for (i = offset = 0; i < ds->unit_count; i++) {
        switch (ds->unit_params[i]->type) {
            case DATA_UNIT_TYPE_ESG:
                sprintf(path, "%s/"DAT_FILE_DIR"/ESG-%d.dat", getenv("HOME"), i);
                int fd = open(path, O_CREAT | O_WRONLY | O_APPEND);
                if (-1 == fd)
                    return obj;

                n = write(fd, ds->data + offset, ds->unit_params[i]->length);
                debug("write bytes %d to file\n", n);
                close(fd);
                break;
            default:
                break;
        }
        offset += ds->unit_params[i]->length;
    }

#endif

    return obj;
}

static MFSParserVisitor visitor = {
    visit_NIT,
    visit_CMCT,
    visit_SMCT,
    visit_CSCT,
    visit_SSCT,
    visit_ESGBDT,
    visit_EB,
    visit_video_section,
    visit_audio_section,
    visit_data_section
};

INLINE void usage(void)
{
    info("\n");
    info("******************************** MFS Parser Usage ******************************\n");
    info("\n");
    info("mfsparser <file> [buffer size]\n\n");
    info("\n");
    info("********************************************************************************\n");
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        error("\ntoo few args\n");
        usage();
        exit(0);
    }

    unsigned char *buf;
    char *path = argv[1];
    long int buf_size = BUF_SIZE;
    int fd_dat = open(argv[1], O_RDONLY);

    if (argc > 2) {
        buf_size = atol(argv[2]);
    }
    info("buffer size:%ld\n", buf_size);

    if (-1 == fd_dat) {
        error("open file %s failed!\n", path);
        exit(0);
    }

    buf = malloc(buf_size);
    ByteStream *stream = stream_new();
    MFSParser *parser = mfsparser_new();

    while (0 < read(fd_dat, buf, buf_size)) {
        stream_write(stream, buf, buf_size);
        mfsparser_parse(parser, stream, &visitor);
    }

    mfsparser_free(&parser);
    stream_free(&stream);

    close(fd_dat);
    free(buf);

    return 0;
}
