#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <log.h>
#include <crc.h>
#include <mfsparser.h>

#undef  __LOG_TAG__
#define __LOG_TAG__         "MFSParser"

#define START_CODE          0x00000001

#ifndef DAT_FILE_DIR
#define DAT_FILE_DIR        ".cmmb"
#endif

#ifndef DAT_FILE_ESGBDT
#define DAT_FILE_ESGBDT     DAT_FILE_DIR"/ESGBDT.dat"
#endif

#ifndef DAT_FILE_MFS_TS0
#define DAT_FILE_MFS_TS0    DAT_FILE_DIR"/TS0.dat"
#endif

#ifndef DAT_FILE_MFS_254
#define DAT_FILE_MFS_254    DAT_FILE_DIR"/MFS-254.dat"
#endif

#ifndef DAT_FILE_ESG
#define DAT_FILE_ESG        DAT_FILE_DIR"/ESG.dat"
#endif

#define PARSE_END_IF(exp) if (exp) goto __PARSE_END__

struct MFSParser
{
    MFSParserState    state;        // 解析器状态
    MuxFrameHeader   *mf_header;    // 复用帧头
    uint32_t          msf_index;    // 复用子帧索引
    NIT              *nit;          // 网络信息表
    CMCT             *cmct;         // 持续时间业务复用配置表
    SMCT             *smct;         // 短时间业务复用配置表
    CSCT             *csct;         // 持续时间业务配置表
    SSCT             *ssct;         // 短时间业务配置表
};

INLINE void write_data(char *path, ByteStream *stream, size_t size)
{
    int fd = open(path, O_WRONLY | O_CREAT);
    if (-1 == fd) {
        error("open file %s failed\n", path);
        return;
    }

    size_t bytes;
    unsigned char data[BUF_SIZE];
    while (size > 0) {
        bytes = MIN(size, BUF_SIZE);
        bytes = stream_reads(stream, data, bytes);
        write(fd, data, bytes);
        size -= BUF_SIZE;
    }

    close(fd);
}

INLINE int locate_mux_frame_header(ByteStream *stream)
{
    uint8_t ui8;
    uint32_t ui32;
    unsigned char cbuf[300];

    while(EOF != stream_read_uint32(stream, &ui32)) {
        if (START_CODE != ui32)
            continue;

        ui8 = stream_read(stream);
        stream_unreads(stream, 5);
        stream_reads(stream, cbuf, ui8 + 4);
        stream_unreads(stream, ui8 + 4);
        if (!crc32(cbuf, ui8 + 4)) {
            debug("find frame header\n");
            return 1;
        }

        stream_read_uint32(stream, &ui32);
        error("CRC error!\n");
    };

    return -1;
}

INLINE NIT* parse_NIT(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor)
{
    register int i;
    uint8_t ui8;
    uint16_t ui16;
    uint32_t ui32;
    size_t bytes = 13;

    NIT *nit = malloc(sizeof(NIT));

    ui8 = stream_read(stream);
    nit->id = ui8;
    debug("NIT.id=0x%x\n", nit->id);

    ui8 = stream_read(stream);
    nit->NIT_update_index = ui8 >> 4;
    debug("NIT.NIT_update_index=%d\n", nit->NIT_update_index);

    stream_read_uint16(stream, &ui16);
    nit->MJD = ui16;
    debug("NIT.MJD=0x%x\n", nit->MJD);

    stream_read_uint24(stream, &ui32);
    nit->BCD = ui32;
    debug("NIT.BCD=0x%x\n", nit->BCD);

    stream_read_uint24(stream, &ui32);
    nit->country_code = ui32;
    debug("NIT.country_code=0x%x\n", nit->country_code);

    stream_read_uint16(stream, &ui16);
    nit->net_level = ui16 >> 12;
    nit->net_id = ui16;
    debug("NIT.net_level=0x%x\n", nit->net_level);
    debug("NIT.net_id=0x%x\n", nit->net_id);

    ui8 = stream_read(stream);
    nit->net_name_length = ui8;
    nit->net_name = malloc(sizeof(uint8_t) * nit->net_name_length);
    stream_reads(stream, nit->net_name, nit->net_name_length);
    debug("NIT.net_name_length=%d\n", nit->net_name_length);
    debug("NIT.net_name=%s\n", nit->net_name);

    bytes += nit->net_name_length;

    ui8 = stream_read(stream);
    nit->freq_point_index = ui8;
    debug("NIT.freq_point_index=%d\n", nit->freq_point_index);

    stream_read_uint32(stream, &ui32);
    nit->central_freq = ui32;
    debug("NIT.central_freq=%d\n", nit->central_freq);

    ui8 = stream_read(stream);
    nit->band_width = ui8 >> 4;
    nit->other_freq_point_count = ui8;
    debug("NIT.band_width=%d\n", nit->band_width);
    debug("NIT.other_freq_point_count=%d\n", nit->other_freq_point_count);

    bytes += 6;

    nit->other_freq_points = malloc(sizeof(void*) * nit->other_freq_point_count);
    for (i = 0; i < nit->other_freq_point_count; i++) {
        FreqPoint *fp = malloc(sizeof(FreqPoint));

        ui8 = stream_read(stream);
        fp->index = ui8;
        debug("FreqPoint[%d].index=%d\n", i, fp->index);

        stream_read_uint32(stream, &ui32);
        fp->central_freq = ui32;
        debug("FreqPoint[%d].central_freq=%d\n", i, fp->central_freq);

        ui8 = stream_read(stream);
        fp->band_width = ui8 >> 4;
        debug("FreqPoint[%d].band_width=%d\n", i, fp->band_width);

        nit->other_freq_points[i] = fp;
        bytes += 6;
    }

    // 邻网
    ui8 = stream_read(stream);
    nit->adjacent_net_count = ui8 >> 4;
    debug("NIT.adjacent_net_count=%d\n", nit->adjacent_net_count);

    bytes += 1;

    nit->adjacent_nets = malloc(sizeof(void*) * nit->adjacent_net_count);
    for (i = 0; i < nit->adjacent_net_count; i++) {
        AdjacentNet *an = malloc(sizeof(AdjacentNet));

        stream_read_uint16(stream, &ui16);
        an->level = ui16 >> 12;
        an->id = ui16;
        debug("AdjacentNet[%d].level=%d\n", i, an->level);
        debug("AdjacentNet[%d].id=%d\n", i, an->id);

        ui8 = stream_read(stream);
        an->freq_point_index = ui8;
        debug("AdjacentNet[%d].freq_point_index=%d\n", i, an->freq_point_index);

        stream_read_uint32(stream, &ui32);
        an->central_freq = ui32;
        debug("AdjacentNet[%d].central_freq=%d\n", i, an->central_freq);

        ui8 = stream_read(stream);
        an->band_width = ui8 >> 4;
        debug("AdjacentNet[%d].band_width=%d\n", i, an->band_width);

        nit->adjacent_nets[i] = an;
        bytes += 8;
    }

    // CRC
    stream_read_uint32(stream, &ui32);
    bytes += 4;

    debug("bytes=%d\n", bytes);

    return nit;
}

INLINE XMCT* parse_XMCT(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor)
{
    register int i, j;

    uint8_t ui8;
    uint16_t ui16;
    uint32_t ui32;
    size_t bytes = 4;

    XMCT *xmct = malloc(sizeof(XMCT));

    ui8 = stream_read(stream);
    xmct->id = ui8;
    debug("XMCT.id=0x%x\n", xmct->id);

    ui8 = stream_read(stream);
    xmct->freq_point_index = ui8;
    debug("XMCT.freq_point_index=%d\n", xmct->freq_point_index);

    stream_read_uint16(stream, &ui16);
    xmct->update_index = ui16 >> 12;
    xmct->mux_frame_count = ui16;
    debug("XMCT.update_index=%d\n", xmct->update_index);
    debug("XMCT.mux_frame_count=%d\n", xmct->mux_frame_count);

    // 复用帧参数
    xmct->mux_frame_params = malloc(sizeof(void*) * xmct->mux_frame_count);
    for (i = 0; i < xmct->mux_frame_count; i++) {
        MuxFrameParam *mfp = malloc(sizeof(MuxFrameParam));

        ui8 = stream_read(stream);
        mfp->id = ui8 >> 2;
        mfp->RS_rate = ui8;
        debug("MuxFrameParam[%d].id=0x%x\n", i, mfp->id);
        debug("MuxFrameParam[%d].RS_rate=0x%x\n", i, mfp->RS_rate);

        stream_read_uint16(stream, &ui16);
        mfp->byte_interleave_mode = ui16 >> 14;
        mfp->LDPC_rate = ui16 >> 12;
        mfp->modulation_method = ui16 >> 10;
        mfp->scramble_method = ui16 >> 6;
        mfp->time_slot_count = ui16;

        debug("MuxFrameParam[%d].byte_interleave_mode=0x%x\n", i, mfp->byte_interleave_mode);
        debug("MuxFrameParam[%d].LDPC_rate=0x%x\n", i, mfp->LDPC_rate);
        debug("MuxFrameParam[%d].modulation_method=0x%x\n", i, mfp->modulation_method);
        debug("MuxFrameParam[%d].scramble_method=0x%x\n", i, mfp->scramble_method);
        debug("MuxFrameParam[%d].time_slot_count=%d\n", i, mfp->time_slot_count);

        bytes += 3;

        // 时隙
        mfp->time_slots = malloc(sizeof(void*) * mfp->time_slot_count);
        for (j = 0; j < mfp->time_slot_count; j++) {
            TimeSlot *ts = malloc(sizeof(TimeSlot));

            ui8 = stream_read(stream);
            ts->index = ui8 >> 2;
            debug("TimeSlot[%d].index=%d\n", j, ts->index);

            mfp->time_slots[j] = ts;
            bytes += 1;
        }

        // 复用子帧参数
        ui8 = stream_read(stream);
        mfp->sub_frame_count = ui8;
        debug("MuxFrameParam[%d].sub_frame_count=0x%x\n", i, mfp->sub_frame_count);

        bytes += 1;

        mfp->sub_frame_params = malloc(sizeof(void*) * mfp->sub_frame_count);
        for (j = 0; j < mfp->sub_frame_count; j++) {
            MuxSubFrameParam *msfp = malloc(sizeof(MuxSubFrameParam));

            ui8 = stream_read(stream);
            msfp->sub_frame_id = ui8 >> 4;
            debug("MuxSubFrameParam[%d].sub_frame_id=0x%x\n", j, msfp->sub_frame_id);

            stream_read_uint16(stream, &ui16);
            msfp->service_id = ui16;
            debug("MuxSubFrameParam[%d].service_id=0x%x\n", j, msfp->service_id);

            bytes += 3;

            mfp->sub_frame_params[j] = msfp;
        }

        xmct->mux_frame_params[i] = mfp;
    }

    // CRC
    stream_read_uint32(stream, &ui32);
    bytes += 4;

    debug("bytes=%d\n", bytes);

    return xmct;
}

INLINE XSCT* parse_XSCT(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor)
{
    register int i;


    uint8_t ui8;
    uint16_t ui16;
    uint32_t ui32;
    size_t bytes = 8;

    XSCT *xsct = malloc(sizeof(XSCT));

    ui8 = stream_read(stream);
    xsct->id = ui8;
    debug("XSCT.id=0x%x\n", xsct->id);

    stream_read_uint16(stream, &ui16);
    xsct->seg_length = ui16;
    debug("XSCT.seg_length=%d\n", xsct->seg_length);

    ui8 = stream_read(stream);
    xsct->seg_index = ui8;
    debug("XSCT.seg_index=%d\n", xsct->seg_index);

    ui8 = stream_read(stream);
    xsct->seg_count = ui8;
    debug("XSCT.seg_count=%d\n", xsct->seg_count);

    ui8 = stream_read(stream);
    xsct->update_index = ui8 >> 4;
    debug("XSCT.update_index=%d\n", xsct->update_index);

    stream_read_uint16(stream, &ui16);
    xsct->service_count = ui16;
    debug("XSCT.service_count=%d\n", xsct->service_count);

    // 业务
    xsct->services = malloc(sizeof(void*) * xsct->service_count);
    for (i = 0; i < xsct->service_count; i++) {
        Service *s = malloc(sizeof(Service));

        stream_read_uint16(stream, &ui16);
        s->id = ui16;
        debug("Service[%d].id=0x%x\n", i, s->id);

        ui8 = stream_read(stream);
        s->freq_point_index = ui8;
        debug("Service[%d].freq_point_index=%d\n", i, s->freq_point_index);

        xsct->services[i] = s;
        bytes += 3;
    }

    // CRC
    stream_read_uint32(stream, &ui32);

    bytes += 4;

    debug("bytes=%d\n", bytes);

    return xsct;
}

INLINE ESGBDT* parse_ESGBDT(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor)
{
    ESGBDT *esgbdt = malloc(sizeof(ESGBDT));
    size_t frm_size = parser->mf_header->sub_frame_lengths[parser->msf_index];

#if 1
    char esg_dat[512];
    unsigned char *buf = malloc(sizeof(unsigned char) * frm_size);

    sprintf(esg_dat, "%s/"DAT_FILE_ESGBDT, getenv("HOME"));

    stream_reads(stream, buf, frm_size);
    int fd = open(esg_dat, O_WRONLY | O_APPEND);
    if (-1 != fd) {
        write(fd, buf, frm_size);
        close(fd);
    }

    free(buf);
#else
    stream_reads(stream, NULL, frm_size);
#endif

    return esgbdt;
}

INLINE EB* parse_EB(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor)
{
    uint8_t ui8;
    uint16_t ui16;
    uint32_t ui32;
    size_t bytes = 4;

    EB *eb = malloc(sizeof(EB));

    ui8 = stream_read(stream);
    eb->id = ui8;
    debug("EB.id=0x%x\n", eb->id);

    ui8 = stream_read(stream);
    eb->index = ui8;
    debug("EB.index=%d\n", eb->index);

    stream_read_uint16(stream, &ui16);
    eb->data_length = ui16;
    debug("EB.data_length=%d\n", eb->data_length);

    // 紧急广播数据
    eb->data = malloc(sizeof(uint8_t) * eb->data_length);
    stream_reads(stream, eb->data, eb->data_length);
    debug("EB.data=%s\n", eb->data);

    bytes += eb->data_length;

    // CRC
    stream_read_uint32(stream, &ui32);
    bytes += 4;

    debug("bytes=%d\n", bytes);

    return eb;
}

INLINE MuxSubFrame* parse_mux_sub_frame(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor)
{
    register int i, length;

#if 0
    size_t frm_size = parser->mf_header->sub_frame_lengths[parser->msf_index];
    char path[255];
    sprintf(path, "%s/"DAT_FILE_DIR"/MSF.dat", getenv("HOME"));
    int fd = open(path, O_CREAT | O_WRONLY);
    if (-1 != fd) {
        unsigned char *data = malloc(sizeof(unsigned char) * frm_size);
        stream_reads(stream, data, frm_size);
        stream_unreads(stream, frm_size);
        write(fd, data, frm_size);
        close(fd);
        free(data);
    }
#endif

    uint8_t ui8;
    uint16_t ui16;
    uint32_t ui32;
    uint8_t cbuf[300];
    size_t bytes = 0;

    // check CRC
    ui8 = stream_read(stream);
    stream_unread(stream);
    stream_reads(stream, cbuf, ui8 + 4);
    stream_unreads(stream, ui8 + 4);

    if (crc32(cbuf, ui8 + 4)) {
        error("CRC error!\n");
        parser->state = STATE_NONE;
        return NULL;
    }

    MuxSubFrame *msf = malloc(sizeof(MuxSubFrame));

    // 复用子帧头
    MuxSubFrameHeader *msfh = malloc(sizeof(MuxSubFrameHeader));

    ui8 = stream_read(stream);
    msfh->length = ui8;
    bytes += 1;
    debug("MuxSubFrameHeader.length=%d\n", msfh->length);

    ui8 = stream_read(stream);
    msfh->start_play_time_flag = ui8 >> 7;
    msfh->video_section_flag = ui8 >> 6;
    msfh->audio_section_flag = ui8 >> 5;
    msfh->data_section_flag = ui8 >> 4;
    msfh->ext_flag = ui8 >> 3;
    msfh->encryption_flag = ui8 >> 1;
    msfh->pkg_mode_flag = ui8;
    bytes += 1;
    debug("MuxSubFrameHeader.start_play_time_flag=0x%x\n", msfh->start_play_time_flag);
    debug("MuxSubFrameHeader.video_section_flag=0x%x\n", msfh->video_section_flag);
    debug("MuxSubFrameHeader.audio_section_flag=0x%x\n", msfh->audio_section_flag);
    debug("MuxSubFrameHeader.data_section_flag=0x%x\n", msfh->data_section_flag);
    debug("MuxSubFrameHeader.ext_flag=0x%x\n", msfh->ext_flag);

    // 起始播放时间
    if (msfh->start_play_time_flag) {
        stream_read_uint32(stream, &ui32);
        msfh->start_play_time = ui32;
        bytes += 4;
    } else {
        msfh->start_play_time = 0;
    }
    debug("MuxSubFrameHeader.start_play_time=0x%x\n", msfh->start_play_time);

    // 视频/音频/数据段大小
    int flags[] = {
        msfh->video_section_flag,
        msfh->audio_section_flag,
        msfh->data_section_flag
    };
    SectionSize **sizes[] = {
        &msfh->video_section_size,
        &msfh->audio_section_size,
        &msfh->data_section_size
    };

    for (i = 0; i < 3; i++) {
        if (flags[i]) {
            SectionSize *ss = malloc(sizeof(SectionSize));

            stream_read_uint24(stream, &ui32);
            ss->length = ui32 >> 3;
            ss->stream_count = ui32;
            bytes += 3;
            debug("SectionSize[%d].length=%d\n", i, ss->length);
            debug("SectionSize[%d].stream_count=0x%x\n", i, ss->stream_count);

            *sizes[i] = ss;
        } else {
            *sizes[i] = NULL;
        }
    }

    // 扩展区
    if (msfh->ext_flag) {

        // 视频流参数
        length = msfh->video_section_size->stream_count;
        msfh->video_stream_params = malloc(sizeof(void*) * length);
        for (i = 0; i < length; i++) {
            VideoStreamParam *vsp = malloc(sizeof(VideoStreamParam));

            ui8 = stream_read(stream);
            vsp->algorithm_type = ui8 >> 5;
            vsp->code_rate_flag = ui8 >> 4;
            vsp->display_flag = ui8 >> 3;
            vsp->resolution_flag = ui8 >> 2;
            vsp->frame_freq_flag = ui8 >> 1;
            bytes += 1;
            debug("VideoStreamParam.algorithm_type=0x%x\n", vsp->algorithm_type);
            debug("VideoStreamParam.code_rate_flag=0x%x\n", vsp->code_rate_flag);
            debug("VideoStreamParam.display_flag=0x%x\n", vsp->display_flag);
            debug("VideoStreamParam.resolution_flag=0x%x\n", vsp->resolution_flag);
            debug("VideoStreamParam.frame_freq_flag=0x%x\n", vsp->frame_freq_flag);

            // 视频码率
            if (vsp->code_rate_flag) {
                stream_read_uint16(stream, &ui16);
                vsp->code_rate = ui16;
                bytes += 2;
            } else {
                vsp->code_rate = 0;
            }
            debug("VideoStreamParam.code_rate=0x%x\n", vsp->code_rate);

            // 图像显示
            if (vsp->display_flag) {
                vsp->display_param = malloc(sizeof(DisplayParam));

                stream_read_uint16(stream, &ui16);
                vsp->display_param->x = ui16 >> 10;
                vsp->display_param->y = ui16 >> 4;
                vsp->display_param->piority = ui16 >> 1;
                bytes += 2;
                debug("DisplayParam.x=%d\n", vsp->display_param->x);
                debug("DisplayParam.y=%d\n", vsp->display_param->y);
                debug("DisplayParam.piority=0x%x\n", vsp->display_param->piority);
            } else {
                vsp->display_param = NULL;
            }

            // 分辨率
            if (vsp->resolution_flag) {
                vsp->resolution_param = malloc(sizeof(ResolutionParam));

                stream_read_uint24(stream, &ui32);
                vsp->resolution_param->width = ui32 >> 10;
                vsp->resolution_param->height = ui32;
                bytes += 3;
                debug("ResolutionParam.width=0x%x\n", vsp->resolution_param->width);
                debug("ResolutionParam.height=0x%x\n", vsp->resolution_param->height);
            } else {
                vsp->resolution_param = NULL;
            }

            // 帧频
            if (vsp->frame_freq_flag) {
                ui8 = stream_read(stream);
                vsp->frame_freq = ui8 >> 4;
                bytes += 1;
                debug("VideoStreamParam.frame_freq=%d\n", vsp->frame_freq);
            } else {
                vsp->frame_freq = 0;
            }

            msfh->video_stream_params[i] = vsp;
        }

        // 音频流参数
        length = msfh->audio_section_size->stream_count;
        msfh->audio_stream_params = malloc(sizeof(void*) * length);
        for (i = 0; i < length; i++) {
            AudioStreamParam *asp = malloc(sizeof(AudioStreamParam));

            ui8 = stream_read(stream);
            asp->algorithm_type = ui8 >> 4;
            asp->code_rate_flag = ui8 >> 3;
            asp->sample_rate_flag = ui8 >> 2;
            asp->desc_flag = ui8 >> 1;
            bytes += 1;
            debug("AudioStreamParam.algorithm_type=0x%x\n", asp->algorithm_type);
            debug("AudioStreamParam.code_rate_flag=0x%x\n", asp->code_rate_flag);
            debug("AudioStreamParam.sample_rate_flag=0x%x\n", asp->sample_rate_flag);
            debug("AudioStreamParam.desc_flag=0x%x\n", asp->desc_flag);

            // 音频码率
            if (asp->code_rate_flag) {
                stream_read_uint16(stream, &ui16);
                asp->code_rate = ui16 >> 2;
                bytes += 2;
                debug("AudioStreamParam.code_rate=0x%x\n", asp->code_rate);
            } else {
                asp->code_rate = 0;
            }

            // 音频采样率
            if (asp->sample_rate_flag) {
                ui8 = stream_read(stream);
                asp->sample_rate = ui8;
                bytes += 1;
                debug("AudioStreamParam.sample_rate=0x%x\n", asp->sample_rate);
            } else {
                asp->sample_rate = 0;
            }

            // 音频流描述
            if (asp->desc_flag) {
                stream_read_uint24(stream, &ui32);
                asp->desc = ui32;
                bytes += 3;
            } else {
                asp->desc = 0;
            }
            debug("AudioStreamParam.desc=0x%x\n", asp->desc);

            msfh->audio_stream_params[i] = asp;
        }
    } else {
        msfh->video_stream_params = NULL;
        msfh->audio_stream_params = NULL;
    }

    // CRC
    stream_read_uint32(stream, &ui32);
    bytes += 4;

    msf->header = msfh;

    // 视频段
    if (msf->header->video_section_flag) {
        VideoSection *vs = malloc(sizeof(VideoSection));

        // 视频段头长度
        stream_read_uint16(stream, &ui16);
        length = ui16 >> 4;
        bytes += 2;
        //vs->header_length = ui16;
        debug("VideoSection.header_length=%d\n", length);

#if 0
        for (i = 0, length = 0; i < vs->header_length - 2; length++) {
            stream_read_uint24(stream, &ui32);
            if (0x00010000 == (ui32 & 0x00010000)) {
                stream_read_uint16(stream, &ui16);
                i += 2;
            }
            length += 3;
        }
        stream_unreads(stream, vs->header_length);
#endif

        // 视频单元参数（按最大单元数算）
        length -= 2; // 去掉视频段头长度
        vs->unit_params = malloc(sizeof(void*) * (length / 3));
        for (i = vs->unit_count = 0; length > 0; vs->unit_count++, i++) {
            VideoUnitParam *vup = malloc(sizeof(VideoUnitParam));

            stream_read_uint16(stream, &ui16);
            vup->length = ui16;
            bytes += 2;
            debug("VideoUnitParam[%d].length=%d\n", i, vup->length);

            ui8 = stream_read(stream);
            vup->frame_type = ui8 >> 5;
            vup->stream_index = ui8 >> 2;
            vup->frame_end_flag = ui8 >> 1;
            vup->play_time_flag = ui8;
            bytes += 1;
            debug("VideoUnitParam[%d].frame_type=0x%x\n", i, vup->frame_type);
            debug("VideoUnitParam[%d].stream_index=%d\n", i, vup->stream_index);
            debug("VideoUnitParam[%d].frame_end_flag=0x%x\n", i, vup->frame_end_flag);
            debug("VideoUnitParam[%d].play_time_flag=0x%x\n", i, vup->play_time_flag);

            //vs->header_length -= 3;
            length -= 3;

            // 相对播放时间
            if (vup->play_time_flag) {
                stream_read_uint16(stream, &ui16);
                vup->play_time = ui16;
                bytes += 2;
                //vs->header_length -= 2;
                length -= 2;
                debug("VideoUnitParam[%d].play_time=%d\n", i, vup->play_time);
            }

            vs->unit_params[i] = vup;
        }
        debug("VideoSection.header_length=%d\n", length);

        // CRC
        stream_read_uint32(stream, &ui32);
        bytes += 4;

        // 视频单元
        for (i = length = 0; i < vs->unit_count; i++) {
            VideoUnitParam *vup = vs->unit_params[i];
            length += vup->length;
        }
        vs->data = malloc(length);
        stream_reads(stream, vs->data, length);
        bytes += length;

        msf->video_section = vs;

        if (visitor && visitor->visit_video_section) {
            visitor->visit_video_section(msfh, msf->video_section, NULL);
        }
    }

    // 音频段
    if (msf->header->audio_section_flag) {
        AudioSection *as = malloc(sizeof(AudioSection));

        ui8 = stream_read(stream);
        as->unit_count = ui8;
        bytes += 1;
        debug("AudioSection.unit_count=%d\n", as->unit_count);

        as->unit_params = malloc(sizeof(void*) * as->unit_count);
        for (i = 0; i < as->unit_count; i++) {
            AudioUnitParam *aup = malloc(sizeof(AudioUnitParam));

            stream_read_uint16(stream, &ui16);
            aup->length = ui16;
            bytes += 2;
            debug("AudioUnitParam[%d].length=%d\n", i, aup->length);

            ui8 = stream_read(stream);
            aup->stream_index = ui8 >> 5;
            bytes += 1;
            debug("AudioUnitParam[%d].stream_index=%d\n", i, aup->stream_index);

            stream_read_uint16(stream, &ui16);
            aup->play_time = ui16;
            bytes += 2;
            debug("AudioUnitParam[%d].play_time=%d\n", i, aup->play_time);

            as->unit_params[i] = aup;
        }

        // CRC
        stream_read_uint32(stream, &ui32);
        bytes += 4;

        // 音频单元
        for (i = length = 0; i < as->unit_count; i++) {
            AudioUnitParam *aup = as->unit_params[i];
            length += aup->length;
        }
        as->data = malloc(length);
        stream_reads(stream, as->data, length);
        bytes += length;

        msf->audio_section = as;

        if (visitor && visitor->visit_audio_section) {
            visitor->visit_audio_section(msfh, msf->audio_section, NULL);
        }
    }

    // 数据段
    if (msf->header->data_section_flag) {
        DataSection *ds = malloc(sizeof(DataSection));

#if 0
        write_data("/home/johnson/"DAT_FILE_ESG, stream, BUF_SIZE);
        exit(0);
#endif

        ui8 = stream_read(stream);
        ds->unit_count = ui8;
        bytes += 1;
        debug("DataSection.unit_count=%d\n", ds->unit_count);

        ds->unit_params = malloc(sizeof(void*) * ds->unit_count);
        for (i = 0; i < ds->unit_count; i++) {
            DataUnitParam *dup = malloc(sizeof(DataUnitParam));

            ui8 = stream_read(stream);
            dup->type = ui8;
            bytes += 1;
            debug("DataUnitParam[%d].type=0x%x\n", i, dup->type);

            stream_read_uint16(stream, &ui16);
            dup->length = ui16;
            bytes += 2;
            debug("DataUnitParam[%d].length=%d\n", i, dup->length);

            ds->unit_params[i] = dup;
        }

        // CRC
        stream_read_uint32(stream, &ui32);
        bytes += 4;

        // 数据单元
        for (i = length = 0; i < ds->unit_count; i++) {
            DataUnitParam *dup = ds->unit_params[i];
            length += dup->length;
        }
        ds->data = malloc(length);
        stream_reads(stream, ds->data, length);
        bytes += length;
        debug("DataSection.length=%d\n", length);

        msf->data_section = ds;

        if (visitor && visitor->visit_data_section) {
            visitor->visit_data_section(msfh, msf->data_section, NULL);
        }
    }

    debug("bytes=%d\n", bytes);

    return msf;
}

MFSParser* mfsparser_new(void)
{
    MFSParser *parser = malloc(sizeof(MFSParser));
    parser->mf_header = NULL;
    parser->state = STATE_NONE;
    parser->msf_index = 0;

    return parser;
}

MuxFrameHeader* mfsparser_parse_header(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor)
{
    register int i;
    uint8_t ui8;
    uint16_t ui16;
    uint32_t ui32;

    if (-1 == locate_mux_frame_header(stream))
        return NULL;

    // 复用帧头
    MuxFrameHeader *mfh = malloc(sizeof(MuxFrameHeader));

    stream_read_uint32(stream, &ui32);
    mfh->start_code = ui32;
    mfh->length = stream_read(stream);
    debug("MuxFrameHeader.start_code=0x%x\n", mfh->start_code);

    stream_read_uint16(stream, &ui16);
    mfh->protocol_version = ui16 >> 11;
    mfh->protocal_min_version = ui16 >> 6;
    mfh->id = ui16;
    debug("MuxFrameHeader.protocol_version=0x%x\n", mfh->protocol_version);
    debug("MuxFrameHeader.protocal_min_version=0x%x\n", mfh->protocal_min_version);
    debug("MuxFrameHeader.id=0x%x\n", mfh->id);

    ui8 = stream_read(stream);
    mfh->EB_flag = ui8 >> 6;
    mfh->NFP_flag = ui8 >> 5;
    mfh->CTUIA_flag = ui8;
    debug("MuxFrameHeader.EB_flag=0x%x\n", mfh->EB_flag);
    debug("MuxFrameHeader.NFP_flag=0x%x\n", mfh->NFP_flag);
    debug("MuxFrameHeader.CTUIA_flag=0x%x\n", mfh->CTUIA_flag);

    ui8 = stream_read(stream);
    mfh->NIT_update_index = ui8 >> 4;
    mfh->CMCT_update_index = ui8;
    debug("MuxFrameHeader.NIT_update_index=%d\n", mfh->NIT_update_index);
    debug("MuxFrameHeader.CMCT_update_index=%d\n", mfh->CMCT_update_index);

    ui8 = stream_read(stream);
    mfh->CSCT_update_index = ui8 >> 4;
    mfh->SMCT_update_index = ui8;
    debug("MuxFrameHeader.CSCT_update_index=%d\n", mfh->CSCT_update_index);
    debug("MuxFrameHeader.SMCT_update_index=%d\n", mfh->SMCT_update_index);

    ui8 = stream_read(stream);
    mfh->SSCT_update_index = ui8 >> 4;
    mfh->ESG_update_index = ui8;
    debug("MuxFrameHeader.SSCT_update_index=%d\n", mfh->SSCT_update_index);
    debug("MuxFrameHeader.ESG_update_index=%d\n", mfh->ESG_update_index);

    ui8 = stream_read(stream);
    mfh->sub_frame_count = ui8;
    debug("MuxFrameHeader.sub_frame_count=%d\n", mfh->sub_frame_count);

    // 复用子帧长度
    mfh->sub_frame_lengths = malloc(sizeof(uint32_t) * mfh->sub_frame_count);
    for (i = 0; i < mfh->sub_frame_count; i++) {
        stream_read_uint24(stream, &ui32);
        mfh->sub_frame_lengths[i] = ui32;
        debug("MuxFrameHeader.sub_frame_lengths[%d]=%d\n", i, ui32);
    }

    // 下一帧参数指示
    if (mfh->NFP_flag) {
        NextFrameParam *nfp = malloc(sizeof(NextFrameParam));

        ui8 = stream_read(stream);
        nfp->header_length = ui8;
        debug("NextFrameParam.header_length=%d\n", nfp->header_length);

        stream_read_uint24(stream, &ui32);
        nfp->frame1_length = ui32;
        debug("NextFrameParam.frame1_length=%d\n", nfp->frame1_length);

        ui8 = stream_read(stream);
        nfp->frame1_header_length = ui8;
        debug("NextFrameParam.frame1_header_length=%d\n", nfp->frame1_header_length);

        mfh->next_frame_params = nfp;
    } else {
        mfh->next_frame_params = NULL;
    }

    // CRC
    stream_read_uint32(stream, &ui32);

    return mfh;
}

void mfsparser_parse(MFSParser *parser, ByteStream *stream, MFSParserVisitor *visitor)
{
    register char cit_id = EOF;
    register size_t stream_size, msf_size;

__PARSE_BEGIN__:

    // parse the frame header
    if (NULL == parser->mf_header || STATE_NONE == parser->state) {
        if (parser->mf_header) {
            mux_frame_header_free(&parser->mf_header);
        }

        if (NULL == (parser->mf_header = mfsparser_parse_header(parser, stream, visitor)))
            return;

        parser->msf_index = 0;
        parser->state = parser->mf_header->id ? STATE_MSF : STATE_CIT;
    }

    // parse the sub frames
    while (parser->msf_index < parser->mf_header->sub_frame_count) {
        MuxFrameHeader *mfh = parser->mf_header;
        stream_size = stream_get_size(stream);
        msf_size = mfh->sub_frame_lengths[parser->msf_index];

        debug("parse sub frame [%d]:%d\n", parser->msf_index, msf_size);

        if (parser->mf_header && stream_size < msf_size) {
            debug("stream size:%d < frame size:%d\n", stream_size, msf_size);
            return;
        }

        switch (parser->state) {
        case STATE_NONE:
            goto __PARSE_BEGIN__;
        case STATE_CIT:
            cit_id = stream_read(stream);

            switch (cit_id) {
            case CIT_NIT:
                stream_unread(stream);

                NIT *nit = parse_NIT(parser, stream, visitor);

                if (visitor && visitor->visit_NIT) {
                    visitor->visit_NIT(nit, NULL);
                }

                nit_free(&nit);
                break;
            case CIT_CMCT:
                stream_unread(stream);

                CMCT *cmct = parse_XMCT(parser, stream, visitor);
                PARSE_END_IF(NULL == cmct);

                if (visitor && visitor->visit_CMCT) {
                    visitor->visit_CMCT(cmct, NULL);
                }

                xmct_free(&cmct);
                break;
            case CIT_CSCT:
                stream_unread(stream);

                CSCT *csct = parse_XSCT(parser, stream, visitor);
                PARSE_END_IF(NULL == csct);

                if (visitor && visitor->visit_CSCT) {
                    visitor->visit_CSCT(csct, NULL);
                }

                xsct_free(&csct);
                break;
            case CIT_SMCT:
                stream_unread(stream);

                SMCT *smct = parse_XMCT(parser, stream, visitor);
                PARSE_END_IF(NULL == smct);

                if (visitor && visitor->visit_SMCT) {
                    visitor->visit_SMCT(smct, NULL);
                }

                xmct_free(&smct);
                break;
            case CIT_SSCT:
                stream_unread(stream);

                SSCT *ssct = parse_XSCT(parser, stream, visitor);
                PARSE_END_IF(NULL == ssct);

                if (visitor && visitor->visit_SSCT) {
                    visitor->visit_SSCT(ssct, NULL);
                }

                xsct_free(&ssct);
                break;
            case CIT_ESGBDT:
                stream_unread(stream);

                ESGBDT *esgbdt = parse_ESGBDT(parser, stream, visitor);
                PARSE_END_IF(NULL == esgbdt);

                if (visitor && visitor->visit_ESGBDT) {
                    visitor->visit_ESGBDT(esgbdt, NULL);
                }

                esgbdt_free(&esgbdt);
                break;
            case CIT_EB:
                stream_unread(stream);

                EB *eb = parse_EB(parser, stream, visitor);
                PARSE_END_IF(NULL == eb);

                if (visitor && visitor->visit_EB) {
                    visitor->visit_EB(eb, NULL);
                }

                eb_free(&eb);
                break;
            default:
                warn("undefined ctrl info table 0x%x\n", cit_id);
                stream_reads(stream, NULL, msf_size - 1);
                warn("skip frame[%d] bytes %d\n", parser->msf_index, msf_size);
                break;
            } // #switch - parse ctrl info table
            break;

        case STATE_MSF:;
            MuxSubFrame *msf = parse_mux_sub_frame(parser, stream, visitor);

            mux_sub_frame_free(&msf);
            break;

        default:
            warn("MFSParser unknown state %d\n", parser->state);
            parser->state = STATE_NONE;
            goto __PARSE_END__;
        } // #switch - parse frame payload

        parser->msf_index++;
    } // #while - multiplex sub frames

    parser->state = STATE_NONE;

__PARSE_END__:
    return;
}

void mfsparser_free(MFSParser **parser)
{
    if ((*parser)->mf_header) {
        mux_frame_header_free(&(*parser)->mf_header);
        (*parser)->mf_header = NULL;
    }

    free(*parser);
    *parser = NULL;
}
