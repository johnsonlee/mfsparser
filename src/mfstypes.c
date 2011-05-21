#include <mfstypes.h>

void mux_frame_header_free(MuxFrameHeader **mfh)
{
    finalize(*mfh);
}

void nit_free(NIT **nit)
{
    finalize(*nit);
}

void xmct_free(XMCT **xmct)
{
    finalize(*xmct);
}

void xsct_free(XSCT **xsct)
{
    finalize(*xsct);
}

void esgbdt_free(ESGBDT **esgbdt)
{
    finalize(*esgbdt);
}

void eb_free(EB **eb)
{
    finalize(*eb);
}

void mux_sub_frame_free(MuxSubFrame **msf)
{
    finalize(*msf);
}

void mux_sub_frame_header_free(MuxSubFrameHeader **msfh)
{
    finalize(*msfh);
}

void video_section_free(VideoSection **vs)
{
    finalize(*vs);
}

void audio_section_free(AudioSection **as)
{
    finalize(*as);
}

void data_section_free(DataSection **ds)
{
    finalize(*ds);
}
