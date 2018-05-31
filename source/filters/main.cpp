#include "wave_filters.h"
#include "edge_filters.h"
int main()
{
    GLRender *gl_ctx = new Edge::EdgeFilter();
    //GLRender *gl_ctx = new Wave::WaveFilter();
    gl_ctx->runDraw();
    delete gl_ctx;

    return 0;
}