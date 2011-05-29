#include <mfstypes.h>

void mux_frame_header_free(MuxFrameHeader **mfh)
{
    if (NULL == *mfh)
        return;

    if ((*mfh)->next_frame_params) {
        free((*mfh)->next_frame_params);
    }

    if ((*mfh)->sub_frame_lengths) {
        free((*mfh)->sub_frame_lengths);
        (*mfh)->sub_frame_count = 0;
        (*mfh)->sub_frame_lengths = NULL;
    }

    free(*mfh);
    *mfh = NULL;
}

void nit_free(NIT **nit)
{
    register int i;

    if (NULL == *nit)
        return;

    if ((*nit)->net_name) {
        free((*nit)->net_name);
        (*nit)->net_name = NULL;
        (*nit)->net_name_length = 0;
    }

    if ((*nit)->other_freq_points) {
        for (i = 0; i < (*nit)->other_freq_point_count; i++) {
            free((*nit)->other_freq_points[i]);
        }
        free((*nit)->other_freq_points);
        (*nit)->other_freq_points = NULL;
    }

    if ((*nit)->adjacent_nets)  {
        for (i = 0; i < (*nit)->adjacent_net_count; i++) {
            free((*nit)->adjacent_nets[i]);
        }
        free((*nit)->other_freq_points);
        (*nit)->other_freq_points = NULL;
    }

    free(*nit);
    *nit = NULL;
}

void xmct_free(XMCT **xmct)
{
    register int i;

    if (NULL == *xmct)
        return;

    if ((*xmct)->mux_frame_params) {
        for (i = 0; i < (*xmct)->mux_frame_count; i++) {
            free((*xmct)->mux_frame_params[i]);
        }
        free((*xmct)->mux_frame_params);
        (*xmct)->mux_frame_params = NULL;
    }

    free(*xmct);
    *xmct = NULL;
}

void xsct_free(XSCT **xsct)
{
    register int i;

    if (NULL == *xsct)
        return;

    if ((*xsct)->services) {
        for (i = 0; i < (*xsct)->service_count; i++) {
            free((*xsct)->services[i]);
        }
        free((*xsct)->services);
        (*xsct)->services = NULL;
    }

    free(*xsct);
    *xsct = NULL;
}

void esgbdt_free(ESGBDT **esgbdt)
{
    if (NULL == *esgbdt)
        return;

    free(*esgbdt);
    *esgbdt = NULL;
}

void eb_free(EB **eb)
{
    if (NULL == *eb)
        return;

    if ((*eb)->data) {
        free((*eb)->data);
        (*eb)->data = NULL;
    }

    free(*eb);
    *eb = NULL;
}

void mux_sub_frame_free(MuxSubFrame **msf)
{
    if (NULL == *msf)
        return;

    register int i;

    if ((*msf)->header) {
        // 视频段
        if ((*msf)->header->video_section_flag)  {
            VideoSection *vs = (*msf)->video_section;

            if (vs->unit_params) {
                for (i = 0; i < vs->unit_count; i++) {
                    free(vs->unit_params[i]);
                }
                free(vs->unit_params);
                vs->unit_params = NULL;
            }

            free((*msf)->video_section);
            (*msf)->video_section = NULL;
        }

        // 音频段
        if ((*msf)->header->audio_section_flag)  {
            AudioSection *as = (*msf)->audio_section;

            if (as->unit_params) {
                for (i = 0; i < as->unit_count; i++) {
                    free(as->unit_params[i]);
                }
                free(as->unit_params);
                as->unit_params = NULL;
            }

            free((*msf)->audio_section);
            (*msf)->audio_section = NULL;
        }

        // 数据段
        if ((*msf)->header->data_section_flag)  {
            DataSection *ds = (*msf)->data_section;

            if (ds->unit_params) {
                for (i = 0; i < ds->unit_count; i++) {
                    free(ds->unit_params[i]);
                }
                free(ds->unit_params);
                ds->unit_params = NULL;
            }

            free((*msf)->data_section);
            (*msf)->data_section = NULL;
        }

        // 子帧头
        if ((*msf)->header->video_stream_params) {
            for (i = 0; i < (*msf)->header->video_section_size->stream_count; i++) {
                VideoStreamParam *vsp = (*msf)->header->video_stream_params[i];

                if (vsp->display_param) {
                    free(vsp->display_param);
                }

                if (vsp->resolution_param) {
                    free(vsp->resolution_param);
                }

                free((*msf)->header->video_stream_params[i]);
            }
            free((*msf)->header->video_stream_params);
            (*msf)->header->video_stream_params = NULL;
        }

        if ((*msf)->header->audio_stream_params) {
            for (i = 0; i < (*msf)->header->audio_section_size->stream_count; i++) {
                free((*msf)->header->audio_stream_params[i]);
            }
            free((*msf)->header->audio_stream_params);
            (*msf)->header->audio_stream_params = NULL;
        }

        if ((*msf)->header->video_section_size) {
            free((*msf)->header->video_section_size);
            (*msf)->header->video_section_size = NULL;
        }

        if ((*msf)->header->audio_section_size) {
            free((*msf)->header->audio_section_size);
            (*msf)->header->audio_section_size = NULL;
        }

        if ((*msf)->header->data_section_size) {
            free((*msf)->header->data_section_size);
            (*msf)->header->data_section_size = NULL;
        }

        free((*msf)->header);
        (*msf)->header = NULL;
    }

    free(*msf);
    *msf = NULL;
}
