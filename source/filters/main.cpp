#include "wave_filters.h"
#include "edge_filters.h"
int main()
{
    GLRender  *gl_ctx = new Edge::EdgeFilter();
    gl_ctx->runDraw();
    delete gl_ctx;

    return 0;
}