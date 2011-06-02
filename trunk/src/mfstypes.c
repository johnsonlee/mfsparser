#include <log.h>
#include <mfstypes.h>

NextFrameParam* next_frame_param_new(void)
{
    NextFrameParam *nfp = malloc(sizeof(NextFrameParam));

    return nfp;
}

void next_frame_param_free(NextFrameParam **nfp)
{
    if (NULL == *nfp)
        return;

    free(*nfp);
    *nfp = NULL;
}

MuxFrameHeader* mux_frame_header_new(void)
{
    MuxFrameHeader *mfh = malloc(sizeof(MuxFrameHeader));

    mfh->NFP_flag = 0;
    mfh->next_frame_params = NULL;
    mfh->sub_frame_count = 0;
    mfh->sub_frame_lengths = NULL;

    return mfh;
}

void mux_frame_header_free(MuxFrameHeader **mfh)
{
    if (NULL == *mfh)
        return;

    if ((*mfh)->next_frame_params) {
        next_frame_param_free(&(*mfh)->next_frame_params);
    }

    if ((*mfh)->sub_frame_lengths) {
        free((*mfh)->sub_frame_lengths);
        (*mfh)->sub_frame_count = 0;
        (*mfh)->sub_frame_lengths = NULL;
    }

    free(*mfh);
    *mfh = NULL;
}

FreqPoint* freq_point_new(void)
{
    FreqPoint *fp = malloc(sizeof(FreqPoint));

    return fp;
}

FreqPoint* freq_point_clone(FreqPoint **clone, FreqPoint *fp)
{
    return *clone;
}

void freq_point_free(FreqPoint **fp)
{
    if (NULL == *fp)
        return;

    free(*fp);
    *fp = NULL;
}

AdjacentNet* adjacent_net_new(void)
{
    AdjacentNet *an = malloc(sizeof(AdjacentNet));

    return an;
}

AdjacentNet* adjacent_net_clone(AdjacentNet **clone, AdjacentNet *an)
{
    return *clone;
}

void adjacent_net_free(AdjacentNet **an)
{
    if (NULL == *an)
        return;

    free(*an);
    *an = NULL;
}

MuxFrameParam* mux_frame_param_new(void)
{
    MuxFrameParam *mfp = malloc(sizeof(MuxFrameParam));

    mfp->time_slots = NULL;
    mfp->time_slot_count = 0;
    mfp->sub_frame_params = NULL;
    mfp->sub_frame_count = 0;

    return mfp;
}

MuxFrameParam* mux_frame_param_clone(MuxFrameParam **clone, MuxFrameParam *mfp)
{
    return *clone;
}

void mux_frame_param_free(MuxFrameParam **mfp)
{
    if (NULL == *mfp)
        return;

    register int i;

    if ((*mfp)->time_slots) {
        for (i = 0; i < (*mfp)->time_slot_count; i++)  {
            time_slot_free(&(*mfp)->time_slots[i]);
        }
    }

    if ((*mfp)->sub_frame_params) {
        for (i = 0; i < (*mfp)->sub_frame_count; i++) {
            mux_sub_frame_param_free(&(*mfp)->sub_frame_params[i]);
        }
    }

    free(*mfp);
    *mfp = NULL;
}

TimeSlot* time_slot_new(void)
{
    TimeSlot *ts = malloc(sizeof(TimeSlot));

    return ts;
}

TimeSlot* time_slot_clone(TimeSlot **clone, TimeSlot *ts)
{
    return *clone;
}

void time_slot_free(TimeSlot **ts)
{
    if (NULL == *ts)
        return;

    free(*ts);
    *ts = NULL;
}

MuxSubFrameParam* mux_sub_frame_param_new(void)
{
    MuxSubFrameParam *msfp = malloc(sizeof(MuxSubFrameParam));

    return msfp;
}

MuxSubFrameParam* mux_sub_frame_param_clone(MuxSubFrameParam **clone, MuxSubFrameParam *msfp)
{
    return *clone;
}

void mux_sub_frame_param_free(MuxSubFrameParam **msfp)
{
    if (NULL == *msfp)
        return;

    free(*msfp);
    *msfp = NULL;
}

Service* service_new(void)
{
    Service *s = malloc(sizeof(Service));

    return s;
}

Service* service_clone(Service **clone, Service *s)
{
    return *clone;
}

void service_free(Service **s)
{
    if (NULL == *s)
        return;

    free(*s);
    *s = NULL;
}

NIT* nit_new(void)
{
    NIT *nit = malloc(sizeof(NIT));

    nit->net_name = NULL;
    nit->net_name_length = 0;
    nit->adjacent_nets = NULL;
    nit->adjacent_net_count = 0;
    nit->other_freq_points = NULL;
    nit->other_freq_point_count = 0;

    return nit;
}

NIT* nit_clone(NIT **clone, NIT *nit)
{
    register int i;
    register size_t offset, size;

    if (NULL == *clone) {
        *clone = nit_new();
    }

    offset = offsetof(NIT, net_name);
    memcpy(*clone, nit, offset);

    if ((*clone)->net_name != NULL) {
        free((*clone)->net_name);
    }

    size = sizeof(uint8_t) * nit->net_name_length;
    (*clone)->net_name = malloc(size);
    memcpy((*clone)->net_name, nit->net_name, size);

    (*clone)->freq_point_index = nit->freq_point_index;
    (*clone)->central_freq = nit->central_freq;
    (*clone)->band_width = nit->band_width;
    (*clone)->other_freq_point_count = nit->other_freq_point_count;
    (*clone)->adjacent_net_count = nit->adjacent_net_count;

    if (NULL != (*clone)->other_freq_points) {
        for (i = 0; i < (*clone)->other_freq_point_count; i++) {
            freq_point_free(&(*clone)->other_freq_points[i]);
        }
        free((*clone)->other_freq_points);
    }

    size = sizeof(FreqPoint*) * nit->other_freq_point_count;
    (*clone)->other_freq_points = malloc(size);
    for (i = 0; i < nit->other_freq_point_count; i++) {
        FreqPoint *fp = freq_point_new();
        memcpy(fp, nit->other_freq_points + i, sizeof(FreqPoint));
        (*clone)->other_freq_points[i] = fp;
    }

    if (NULL != (*clone)->adjacent_nets) {
        for (i = 0; i < (*clone)->adjacent_net_count; i++) {
            adjacent_net_free(&(*clone)->adjacent_nets[i]);
        }
        free((*clone)->adjacent_nets);
    }

    size = sizeof(AdjacentNet*) * nit->adjacent_net_count;
    (*clone)->adjacent_nets = malloc(size);
    for (i = 0; i < nit->adjacent_net_count; i++) {
        AdjacentNet *an = adjacent_net_new();
        memcpy(an, nit->adjacent_nets + i, sizeof(AdjacentNet));
        (*clone)->adjacent_nets[i] = an;
    }

    return *clone;
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
            freq_point_free(&(*nit)->other_freq_points[i]);
        }
        free((*nit)->other_freq_points);
        (*nit)->other_freq_points = NULL;
    }

    if ((*nit)->adjacent_nets)  {
        for (i = 0; i < (*nit)->adjacent_net_count; i++) {
            adjacent_net_free(&(*nit)->adjacent_nets[i]);
        }
        free((*nit)->other_freq_points);
        (*nit)->other_freq_points = NULL;
    }

    free(*nit);
    *nit = NULL;
}

XMCT* xmct_new(void)
{
    XMCT *xmct = malloc(sizeof(XMCT));

    xmct->mux_frame_count = 0;
    xmct->mux_frame_params = NULL;

    return xmct;
}

XMCT* xmct_clone(XMCT **clone, XMCT *xmct)
{
    register int i, j;
    register size_t offset, size;

    if (NULL == *clone) {
        *clone = xmct_new();
    }

    offset = offsetof(XMCT, mux_frame_params);
    memcpy(*clone, xmct, offset);

    if (NULL != (*clone)->mux_frame_params) {
        for (i = 0; i < (*clone)->mux_frame_count; i++) {
            MuxFrameParam *mfp = (*clone)->mux_frame_params[i];

            for (j = 0; j < mfp->time_slot_count; j++) {
                time_slot_free(&mfp->time_slots[j]);
            }

            for (j = 0; j < mfp->sub_frame_count; j++) {
                mux_sub_frame_param_free(&(mfp->sub_frame_params[j]));
            }

            mux_frame_param_free(&mfp);
        }
        free((*clone)->mux_frame_params);
    }

    size = sizeof(MuxFrameParam*) * xmct->mux_frame_count;
    (*clone)->mux_frame_params = malloc(size);
    for (i = 0; i < xmct->mux_frame_count; i++) {
        MuxFrameParam *mfp_src = xmct->mux_frame_params[i];
        MuxFrameParam *mfp_dest = mux_frame_param_new();

        offset = offsetof(MuxFrameParam, time_slots);
        memcpy(mfp_dest, mfp_src, offset);
        mfp_dest->sub_frame_count = mfp_src->sub_frame_count;

        for (j = 0; j < mfp_dest->time_slot_count; j++)  {
            mfp_dest->time_slots[j] = time_slot_new();
            memcpy(mfp_dest->time_slots + j, mfp_src->time_slots + j, sizeof(TimeSlot));
        }

        for (j = 0; j < mfp_dest->sub_frame_count; j++) {
            mfp_dest->sub_frame_params[j] = mux_sub_frame_param_new();
            memcpy(mfp_dest->sub_frame_params + j, mfp_src->sub_frame_params + j, sizeof(MuxSubFrameParam));
        }

        (*clone)->mux_frame_params[i] = mfp_dest;
    }

    return *clone;
}

void xmct_free(XMCT **xmct)
{
    register int i;

    if (NULL == *xmct)
        return;

    if ((*xmct)->mux_frame_params) {
        for (i = 0; i < (*xmct)->mux_frame_count; i++) {
            mux_frame_param_free(&(*xmct)->mux_frame_params[i]);
        }
        free((*xmct)->mux_frame_params);
        (*xmct)->mux_frame_params = NULL;
    }

    free(*xmct);
    *xmct = NULL;
}

XSCT* xsct_new(void)
{
    XSCT *xsct = malloc(sizeof(XSCT));

    xsct->services = NULL;
    xsct->service_count = 0;

    return xsct;
}

XSCT* xsct_clone(XSCT **clone, XSCT *xsct)
{
    register int i;
    register size_t offset;

    if (NULL == *clone) {
        *clone = xsct_new();
    }

    offset = offsetof(XSCT, services);
    memcpy(*clone, xsct, offset);

    if (NULL != (*clone)->services) {
        for (i = 0; i < (*clone)->service_count; i++) {
            service_free(&(*clone)->services[i]);
        }
        free((*clone)->services);
    }

    (*clone)->services = malloc(sizeof(Service*) * xsct->service_count);
    for (i = 0; i < xsct->service_count; i++) {
        (*clone)->services[i] = service_new();
        memcpy((*clone)->services + i, xsct->services + i, sizeof(Service));
    }

    return *clone;
}

void xsct_free(XSCT **xsct)
{
    register int i;

    if (NULL == *xsct)
        return;

    if ((*xsct)->services) {
        for (i = 0; i < (*xsct)->service_count; i++) {
            service_free(&(*xsct)->services[i]);
        }
        free((*xsct)->services);
        (*xsct)->services = NULL;
    }

    free(*xsct);
    *xsct = NULL;
}

ESGBDT* esgbdt_new(void)
{
    ESGBDT *esgbdt = malloc(sizeof(ESGBDT));

    return esgbdt;
}

ESGBDT* esgbdt_clone(ESGBDT **clone, ESGBDT *esgbdt)
{
    if (NULL == *clone) {
        *clone = esgbdt_new();
    }

    // TODO clone fields of ESGBDT

    return *clone;
}

void esgbdt_free(ESGBDT **esgbdt)
{
    if (NULL == *esgbdt)
        return;

    free(*esgbdt);
    *esgbdt = NULL;
}

EB* eb_new(void)
{
    EB *eb = malloc(sizeof(EB));

    eb->data = NULL;
    eb->data_length = 0;

    return eb;
}

void eb_free(EB **eb)
{
    if (NULL == *eb)
        return;

    if ((*eb)->data) {
        free((*eb)->data);
    }

    free(*eb);
    *eb = NULL;
}

EADT* eadt_new(void)
{
    EADT *eadt = malloc(sizeof(EADT));

    return eadt;
}

EADT* eadt_clone(EADT **clone, EADT *eadt)
{
    return *clone;
}

void eadt_free(EADT **eadt)
{
    if (NULL == *eadt)
        return;

    free(*eadt);
    *eadt = NULL;
}

CIDT* cidt_new(void)
{
    CIDT *cidt = malloc(sizeof(CIDT));

    cidt->comm_infos = NULL;
    cidt->data_length = 0;

    return cidt;
}

void cidt_free(CIDT **ci)
{
    if (NULL == *ci)
        return;

    register int i;

    if ((*ci)->comm_infos) {
        for (i = 0; i < (*ci)->ci_count; i++) {
            common_info_free(&(*ci)->comm_infos[i]);
        }
        free((*ci)->comm_infos);
        (*ci)->comm_infos = NULL;
    }

    free(*ci);
    *ci = NULL;
}

CommonInfo* common_info_new(void)
{
    CommonInfo *ci = malloc(sizeof(CommonInfo));

    ci->custom_name = NULL;
    ci->custom_name_length = 0;
    ci->trig_msg = NULL;

    return ci;
}

CommonInfo* common_info_clone(CommonInfo **clone, CommonInfo *ci)
{
    return *clone;
}

void common_info_free(CommonInfo **ci)
{
    if (NULL == *ci)
        return;

    if ((*ci)->custom_name) {
        free((*ci)->custom_name);
        (*ci)->custom_name = NULL;
    }

    if ((*ci)->trig_msg)  {
        free((*ci)->trig_msg);
        (*ci)->trig_msg = NULL;
    }

    free(*ci);
    *ci = NULL;
}

EBDataSection* eb_data_section_new(void)
{
    EBDataSection *ebds = malloc(sizeof(EBDataSection));

    ebds->data = NULL;
    ebds->data_length = 0;

    return ebds;
}

void eb_data_section_free(EBDataSection **ebds)
{
    if (NULL == ebds)
        return;

    if ((*ebds)->data) {
        free((*ebds)->data);
        (*ebds)->data = NULL;
    }

    free(*ebds);
    *ebds = NULL;
}

EBMessage* eb_message_new(void)
{
    EBMessage *ebmsg = malloc(sizeof(EBMessage));

    ebmsg->msg_param = NULL;
    ebmsg->trig_param = NULL;

    return ebmsg;
}

void eb_message_free(EBMessage **ebmsg)
{
    if (NULL == *ebmsg)
        return;

    if ((*ebmsg)->trig_flag) {
        eb_trig_param_free(&(*ebmsg)->trig_param);
        (*ebmsg)->trig_param = NULL;
    } else {
        eb_message_param_free(&(*ebmsg)->msg_param);
        (*ebmsg)->msg_param = NULL;
    }

    free(*ebmsg);
    *ebmsg = NULL;
}

EBTrigParam* eb_trig_param_new(void)
{
    EBTrigParam *ebtp = malloc(sizeof(EBTrigParam));

    return ebtp;
}

void eb_trig_param_free(EBTrigParam **ebtp)
{
    if (NULL == *ebtp)
        return;

    free(*ebtp);
    *ebtp = NULL;
}

EBMessageParam* eb_message_param_new(void)
{
    EBMessageParam *ebmp = malloc(sizeof(EBMessageParam));

    return ebmp;
}

void eb_message_param_free(EBMessageParam **ebmp)
{
    if (NULL == *ebmp)
        return;

    free(*ebmp);
    *ebmp = NULL;
}

MuxSubFrame* mux_sub_frame_new(void)
{
    MuxSubFrame *msf = malloc(sizeof(MuxSubFrame));

    msf->header = NULL;
    msf->video_section = NULL;
    msf->audio_section = NULL;
    msf->data_section = NULL;

    return msf;
}

void mux_sub_frame_free(MuxSubFrame **msf)
{
    if (NULL == *msf)
        return;

    if ((*msf)->header) {
        if ((*msf)->header->video_section_flag) {
            video_section_free(&(*msf)->video_section);
        }

        if ((*msf)->header->audio_section_flag) {
            audio_section_free(&(*msf)->audio_section);
        }

        if ((*msf)->header->data_section_flag) {
            data_section_free(&(*msf)->data_section);
        }
        mux_sub_frame_header_free(&(*msf)->header);
    }

    free(*msf);
    *msf = NULL;
}

MuxSubFrameHeader* mux_sub_frame_header_new(void)
{
    MuxSubFrameHeader *msfh = malloc(sizeof(MuxSubFrameHeader));

    msfh->video_section_size = NULL;
    msfh->audio_section_size = NULL;
    msfh->data_section_size = NULL;
    msfh->audio_stream_params = NULL;
    msfh->video_stream_params = NULL;

    return msfh;
}
void mux_sub_frame_header_free(MuxSubFrameHeader **msfh)
{
    if (NULL == *msfh)
        return;

    register int i;

    if ((*msfh)->video_stream_params) {
        for (i = 0; i < (*msfh)->video_section_size->stream_count; i++) {
            video_stream_param_free(&(*msfh)->video_stream_params[i]);
        }
        free((*msfh)->video_stream_params);
        (*msfh)->video_stream_params = NULL;
    }

    if ((*msfh)->audio_stream_params) {
        for (i = 0; i < (*msfh)->audio_section_size->stream_count; i++) {
            audio_stream_param_free(&(*msfh)->audio_stream_params[i]);
        }
        free((*msfh)->audio_stream_params);
        (*msfh)->audio_stream_params = NULL;
    }

    if ((*msfh)->video_section_size) {
        section_size_free(&(*msfh)->video_section_size);
    }

    if ((*msfh)->audio_section_size) {
        section_size_free(&(*msfh)->audio_section_size);
    }

    if ((*msfh)->data_section_size) {
        section_size_free(&(*msfh)->data_section_size);
    }

    free(*msfh);
    *msfh = NULL;
}

VideoSection* video_section_new(void)
{
    VideoSection *vs = malloc(sizeof(VideoSection));

    vs->data = NULL;
    vs->unit_count = 0;
    vs->unit_params = NULL;

    return vs;
}

void video_section_free(VideoSection **vs)
{
    if (NULL == *vs)
        return;

    register int i;

    if ((*vs)->unit_params) {
        for (i = 0; i < (*vs)->unit_count; i++) {
            video_unit_param_free(&(*vs)->unit_params[i]);
        }
        free((*vs)->unit_params);
        (*vs)->unit_params = NULL;
    }

    if ((*vs)->data) {
        free((*vs)->data);
        (*vs)->data = NULL;
    }

    free(*vs);
    *vs = NULL;
}

AudioSection* audio_section_new(void)
{
    AudioSection *as = malloc(sizeof(AudioSection));

    as->data = NULL;
    as->unit_count = 0;
    as->unit_params = NULL;

    return as;
}

void audio_section_free(AudioSection **as)
{
    if (NULL == *as)
        return;

    register int i;

    if ((*as)->unit_params) {
        for (i = 0; i < (*as)->unit_count; i++) {
            audio_unit_param_free(&(*as)->unit_params[i]);
        }
        free((*as)->unit_params);
        (*as)->unit_params = NULL;
    }

    free(*as);
    *as = NULL;
}

DataSection* data_section_new(void)
{
    DataSection *ds = malloc(sizeof(DataSection));

    ds->data = NULL;
    ds->unit_count = 0;
    ds->unit_params = NULL;

    return ds;
}

void data_section_free(DataSection **ds)
{
    if (NULL == *ds)
        return;

    register int i = 0;

    if ((*ds)->unit_params) {
        for (i = 0; i < (*ds)->unit_count; i++) {
            data_unit_param_free(&(*ds)->unit_params[i]);
        }
        free((*ds)->unit_params);
        (*ds)->unit_params = NULL;
    }

    if ((*ds)->data) {
        free((*ds)->data);
        (*ds)->data = NULL;
    }

    free(*ds);
    *ds = NULL;
}

SectionSize* section_size_new(void)
{
    SectionSize *ss = malloc(sizeof(SectionSize));

    return ss;
}

void section_size_free(SectionSize **ss)
{
    if (NULL == *ss)
        return;

    free(*ss);
    *ss = NULL;
}

DisplayParam* display_param_new(void)
{
    DisplayParam *dp = malloc(sizeof(DisplayParam));

    return dp;
}

void display_param_free(DisplayParam **dp)
{
    if (NULL == *dp)
        return;

    free(*dp);
    *dp = NULL;
}

ResolutionParam* resolution_param_new(void)
{
    ResolutionParam *rp = malloc(sizeof(ResolutionParam));

    return rp;
}

void resolution_param_free(ResolutionParam **rp)
{
    if (NULL == *rp)
        return;

    free(*rp);
    *rp = NULL;
}

VideoStreamParam* video_stream_param_new(void)
{
    VideoStreamParam *vsp = malloc(sizeof(VideoStreamParam));

    vsp->display_flag = 0;
    vsp->display_param = NULL;
    vsp->resolution_flag = 0;
    vsp->resolution_param = NULL;

    return vsp;
}

void video_stream_param_free(VideoStreamParam **vsp)
{
    if (NULL == *vsp)
        return;

    if ((*vsp)->display_param) {
        display_param_free(&(*vsp)->display_param);
        (*vsp)->display_param = NULL;
    }

    if ((*vsp)->resolution_param)  {
        resolution_param_free(&(*vsp)->resolution_param);
        (*vsp)->display_param = NULL;
    }

    free(*vsp);
    *vsp = NULL;
}

AudioStreamParam* audio_stream_param_new(void)
{
    AudioStreamParam *asp = malloc(sizeof(AudioStreamParam));

    return asp;
}

void audio_stream_param_free(AudioStreamParam **asp)
{
    if (NULL == *asp)
        return;

    free(*asp);
    *asp = NULL;
}

VideoUnitParam* video_unit_param_new(void)
{
    VideoUnitParam *vup = malloc(sizeof(VideoUnitParam));

    return vup;
}

void video_unit_param_free(VideoUnitParam **vup)
{
    if (NULL == *vup)
        return;

    free(*vup);
    *vup = NULL;
}

AudioUnitParam* audio_unit_param_new(void)
{
    AudioUnitParam *aup = malloc(sizeof(AudioUnitParam));

    return aup;
}

void audio_unit_param_free(AudioUnitParam **aup)
{
    if (NULL == *aup)
        return;

    free(*aup);
    *aup = NULL;
}

DataUnitParam* data_unit_param_new(void)
{
    DataUnitParam *dup = malloc(sizeof(DataUnitParam));

    return dup;
}

void data_unit_param_free(DataUnitParam **dup)
{
    if (NULL == *dup)
        return;

    free(*dup);
    *dup = NULL;
}
