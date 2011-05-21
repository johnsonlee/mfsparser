#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>
#include <crc.h>
#include <stack.h>
#include <mfsparser.h>

const size_t ptr_size = sizeof(void*);
const unsigned char mfsh_mark[] = { 0x00, 0x00, 0x00, 0x01 };

static int find_mux_frame_header(ByteBuffer *buf)
{
    register int i = buf->offset;
    for (; i < buf->size && (buf->size - i >= 4); i += 4) {
        if (!memcmp(mfsh_mark, buf->data + i, 4)) {
            debug("find multiplex frame header at %d\n", i);
            if (!crc32(buf->data + i, buf->data[i + 4] + 4)) {
                return i;
            }
            debug("check crc32 error\n");
        }
    }

    return -1;
}

static int locate_mux_frame_header(ByteBuffer *buf)
{
    int i = -1;

    if (-1 != (i = find_mux_frame_header(buf))) {
        buf->offset = i;
    }

    return i;
}

static MuxFrameHeader* parse_mux_frame_header(ByteBuffer *buf)
{
    register int i, index = locate_mux_frame_header(buf);

    if (-1 == index) {
        return NULL;
    }

    MuxFrameHeader *mfh = construct(MuxFrameHeader);
    memcpy(mfh, buf->data + buf->offset, 12);
    mfh->sub_frame_lengths = nconstruct(uint32_t, mfh->sub_frame_number);
    buf->offset += 12;

    for (i = 0; i < mfh->sub_frame_number; i++) {
        mfh->sub_frame_lengths[i] = ((buf->data[buf->offset++] & 0xFF) << 0x10)
                                  | ((buf->data[buf->offset++] & 0xFF) << 0x08)
                                  | ((buf->data[buf->offset++] & 0xFF) << 0x00);
    }

    if (mfh->NFP_flag) {
        mfh->next_frame_params = construct(NextFrameParam);
        memcpy(mfh->next_frame_params, buf->data + buf->offset, 5);
        buf->offset += 5;
    } else {
        mfh->next_frame_params = NULL;
    }

    buf->offset += 4; // CRC

    return mfh;
}

static NIT* parse_NIT(ByteBuffer *buf)
{
    register int i;

    NIT *nit = construct(NIT);

    memcpy(nit, buf->data + buf->offset, 12);
    buf->offset += 12;
    nit->net_name_length = buf->data[buf->offset++];
    nit->net_name = nconstruct(uint8_t, nit->net_name_length);
    memcpy(nit->net_name, buf->data + buf->offset, nit->net_name_length);
    buf->offset += nit->net_name_length;
    nit->freq_point_index = buf->data[buf->offset++];
    nit->central_freq = ((buf->data[buf->offset++] & 0xFF) << 0x18)
                      | ((buf->data[buf->offset++] & 0xFF) << 0x10)
                      | ((buf->data[buf->offset++] & 0xFF) << 0x08)
                      | ((buf->data[buf->offset++] & 0xFF) << 0x00);
    nit->band_width = buf->data[buf->offset] & 0x0F;
    nit->other_freq_point_number = buf->data[buf->offset++] >> 4;
    nit->other_freq_points = nconstruct(FreqPoint, nit->other_freq_point_number);

    for (i = 0; i < nit->other_freq_point_number; i++) {
        FreqPoint *fp = nit->other_freq_points[i];
        memcpy(fp, buf->data + buf->offset, 6);
        buf->offset += 6;
    }

    nit->adjacent_net_number = buf->data[buf->offset++] & 0x0F;
    nit->adjacent_nets = nconstruct(AdjacentNet, nit->adjacent_net_number);
    size_t ans_size = sizeof(AdjacentNet) * nit->adjacent_net_number;
    memcpy(nit->adjacent_nets, buf->data + buf->offset, ans_size);
    buf->offset += ans_size;

    buf->offset += 4; // CRC

    return nit;
}

static XMCT* parse_XMCT(ByteBuffer *buf)
{
    register int i, j;

    XMCT *xmct = construct(XMCT);

    memcpy(xmct, buf->data + buf->offset, 4);
    buf->offset += 4;

    xmct->mux_frame_params = nconstruct(MuxFrameParam, xmct->mux_frame_number);

    for (i = 0; i < xmct->mux_frame_number; i++) {
        MuxFrameParam *mfp = xmct->mux_frame_params[i];
        memcpy(mfp, buf->data + buf->offset, 3);
        buf->offset += 3;

        size_t tss_size = mfp->time_slot_number * sizeof(TimeSlot);
        mfp->time_slots = nconstruct(TimeSlot, mfp->time_slot_number);
        memcpy(mfp->time_slots, buf->data + buf->offset, tss_size);
        buf->offset += tss_size;

        mfp->sub_frame_number = buf->data[buf->offset++] >> 4;
        mfp->sub_frame_params = nconstruct(MuxSubFrameParam, mfp->sub_frame_number);
        for (j = 0; j < mfp->sub_frame_number; j++) {
            memcpy(mfp->sub_frame_params[i], buf->data + buf->offset, 3);
            buf->offset += 3;
        }
    }

    buf->offset += 4; // CRC

    return xmct;
}

static XSCT* parse_XSCT(ByteBuffer *buf)
{
    register int i;

    XSCT *xsct = construct(XSCT);

    memcpy(xsct, buf->data + buf->offset, 8);
    buf->offset += 8;

    xsct->services = nconstruct(Service, xsct->service_number);

    for (i = 0; i < xsct->service_number; i++) {
        memcpy(xsct->services[i], buf->data + buf->offset, 3);
        buf->offset += 3;
    }

    buf->offset += 4; // CRC

    return xsct;
}

static EB* parse_EB(ByteBuffer *buf)
{
    EB *eb = construct(EB);

    memcpy(eb, buf->data + buf->offset, 4);
    buf->offset += 4;

    memcpy(eb->data, buf->data + buf->offset, eb->data_length);
    buf->offset += eb->data_length;

    buf->offset += 4; // CRC

    return eb;
}

static MuxSubFrameHeader* parse_mux_sub_frame_header(ByteBuffer *buf)
{
    register int i, length;

    MuxSubFrameHeader *msfh = construct(MuxSubFrameHeader);

    memcpy(msfh, buf->data + buf->offset, 2);
    buf->offset += 2;

    if (msfh->start_play_time_flag) {
        msfh->start_play_time = ((buf->data[buf->offset++] & 0xFF) << 0x18)
                              | ((buf->data[buf->offset++] & 0xFF) << 0x10)
                              | ((buf->data[buf->offset++] & 0xFF) << 0x08)
                              | ((buf->data[buf->offset++] & 0xFF) << 0x00);
    } else {
        msfh->start_play_time = 0;
    }

    // video/audio/data section size
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
            SectionSize *ss = construct(SectionSize);
            ss->length = ((buf->data[buf->offset++] & 0x1F) << 0x10)
                       | ((buf->data[buf->offset++] & 0xFF) << 0x08)
                       | ((buf->data[buf->offset++] & 0xFF) << 0x00);
            ss->stream_number = buf->data[buf->offset - 1] >> 5;
            *sizes[i] = ss;
        } else {
            *sizes[i] = NULL;
        }
    }

    // extension area
    if (msfh->ext_flag) {
        length = msfh->video_section_size->length;
        msfh->video_stream_params = nconstruct(VideoStreamParam, length);
        for (i = 0; i < length; i++) {
            VideoStreamParam *vsp = msfh->video_stream_params[i];
            memcpy(vsp, buf->data + buf->offset, 1);
            buf->offset += 1;

            if (vsp->code_rate_flag) {
                vsp->code_rate = ((buf->data[buf->offset++] & 0xFF) << 0x08)
                               | ((buf->data[buf->offset++] & 0xFF) << 0x00);
            } else {
                vsp->code_rate = 0;
            }

            if (vsp->display_flag) {
                vsp->display_param = construct(DisplayParam);
                memcpy(vsp->display_param, buf->data + buf->offset, 2);
                buf->offset += 2;
            } else  {
                vsp->display_param = NULL;
            }

            if (vsp->resolution_flag) {
                vsp->resolution_param = construct(ResolutionParam);
                memcpy(vsp->resolution_param, buf->data + buf->offset, 3);
                buf->offset += 3;
            } else {
                vsp->resolution_param = NULL;
            }
        }

        length = msfh->audio_section_size->length;
        msfh->audio_stream_params = nconstruct(AudioStreamParam, length);
        for (i = 0; i < length; i++) {
            AudioStreamParam *asp = msfh->audio_stream_params[i];
            memcpy(asp, buf->data + buf->offset, 1);
            buf->offset += 1;

            if (asp->code_rate_flag) {
                asp->code_rate = ((buf->data[buf->offset++] & 0x3F) << 0x08)
                               | ((buf->data[buf->offset++] & 0xFF) << 0x00);
            } else {
                asp->code_rate = 0;
            }

            if (asp->sample_rate_flag) {
                asp->sample_rate = buf->data[buf->offset++] >> 4;
            } else {
                asp->sample_rate = 0;
            }

            if (asp->desc_flag)  {
                asp->desc = ((buf->data[buf->offset++] & 0xFF) << 0x10)
                          | ((buf->data[buf->offset++] & 0xFF) << 0x08)
                          | ((buf->data[buf->offset++] & 0xFF) << 0x00);
            } else {
                asp->desc = 0;
            }
        }
    } else  {
        msfh->video_stream_params = NULL;
        msfh->audio_stream_params = NULL;
    }

    buf->offset += 4; // CRC

    return msfh;
}

static VideoSection* parse_video_section(ByteBuffer *buf, int *n)
{
    register int i, length;

    VideoSection *vs = construct(VideoSection);
    vs->header_length = ((buf->data[buf->offset++] & 0x0F) << 0x08)
                      | ((buf->data[buf->offset++] & 0xFF) << 0x00);

    for (i = 0, length = 0; i < vs->header_length - 2; length++) {
        if (buf->data[buf->offset + i + 2] & 0x01) {
            i += 2;
        }
        i += 3;
    }
    *n = length;

    vs->units = nconstruct(ByteBuffer, length);
    vs->unit_params = nconstruct(AudioUnitParam, length);
    for (i = 0; i < length; i++) {
        VideoUnitParam *vup = vs->unit_params[i];
        vup->length = ((buf->data[buf->offset++] & 0xFF) << 0x08)
                    | ((buf->data[buf->offset++] & 0xFF) << 0x00);
        vup->frame_type = buf->data[buf->offset] & 0x07;
        vup->stream_index = buf->data[buf->offset] & 0x38 >> 3;
        vup->frame_end_flag = buf->data[buf->offset] & 0x40 >> 6;
        vup->play_time_flag = buf->data[buf->offset++] >> 7;
    }

    buf->offset += 4; // CRC

    for (i = 0; i < length; i++) {
        ByteBuffer *bb = vs->units[i];
        bb->offset = 0;
        bb->size = vs->unit_params[i]->length;
        bb->data = nconstruct(uint8_t, bb->size);
        memcpy(bb->data, buf->data + buf->offset, bb->size);
        buf->offset += bb->size;
    }

    return vs;
}

static AudioSection* parse_audio_section(ByteBuffer *buf)
{
    register int i;

    AudioSection *as = construct(AudioSection);
    as->unit_number = buf->data[buf->offset++];
    as->unit_params = nconstruct(AudioUnitParam, as->unit_number);
    as->units = nconstruct(ByteBuffer, as->unit_number);
    for (i = 0; i < as->unit_number; i++) {
        AudioUnitParam *aup = as->unit_params[i];
        aup->length = ((buf->data[buf->offset++] & 0xFF) << 0x08)
                    | ((buf->data[buf->offset++] & 0xFF) << 0x00);
        aup->stream_index = buf->data[buf->offset++] & 0x07;
        aup->play_time = ((buf->data[buf->offset++] & 0xFF) << 0x08)
                       | ((buf->data[buf->offset++] & 0xFF) << 0x00);
    }

    buf->offset += 4; // CRC

    for (i = 0; i < as->unit_number; i++) {
        ByteBuffer *bb = as->units[i];
        bb->offset = 0;
        bb->size = as->unit_params[i]->length;
        bb->data = nconstruct(uint8_t, bb->size);
        memcpy(bb->data, buf->data + buf->offset, bb->size);
        buf->offset += bb->size;
    }

    return as;
}

static DataSection* parse_data_section(ByteBuffer *buf)
{
    register int i;

    DataSection *ds = construct(DataSection);
    ds->unit_number = buf->data[buf->offset++];
    ds->unit_params = nconstruct(DataUnitParam, ds->unit_number);
    ds->units = nconstruct(ByteBuffer, ds->unit_number);

    for (i = 0; i < ds->unit_number; i++) {
        DataUnitParam *dup = ds->unit_params[i];
        dup->type = buf->data[buf->offset++];
        dup->length = ((buf->data[buf->offset++] & 0xFF) << 0x08)
                    | ((buf->data[buf->offset++] & 0xFF) << 0x00);
    }

    buf->offset += 4; // CRC

    for (i = 0; i < ds->unit_number; i++) {
        ByteBuffer *bb = ds->units[i];
        bb->offset = 0;
        bb->size = ds->unit_params[i]->length;
        bb->data = nconstruct(uint8_t, bb->size);
        memcpy(bb->data, buf->data + buf->offset, bb->size);
        buf->offset += bb->size;
    }

    return ds;
}

void parse_mux_frame(ByteBuffer *buf, MFSParserVisitor *visitor)
{
    static MFSParserState state = STATE_NONE;

    switch (state) {
    case STATE_NONE:
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    return 0;
}
