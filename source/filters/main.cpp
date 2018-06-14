#include "wave_filters.h"
#include "edge_filters.h"
#include "mosaic _filters.h"
#include "CMYK_color.h"
#include "HSV_color.h"
#include "hdr.h"
int main()
{
    GLRender *gl_ctx = nullptr;
    //gl_ctx = new Mosaic::MosaicFilter();
    //gl_ctx = new Edge::EdgeFilter();
    //gl_ctx = new Wave::WaveFilter();
    //gl_ctx = new CMYK::CMYKFilter();
    //gl_ctx = new HSV::HSVFilter();
    gl_ctx = new HDR::HDRFilter();
    gl_ctx->runDraw();
    delete gl_ctx;

    return 0;
}