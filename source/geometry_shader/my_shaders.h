#ifndef SHADERS_H_
#define SHADERS_H_

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
#include "gl_render.h"
#include "learnopengl/shaders_all.h"
#undef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const char vertexShaderSource[] = R"(
#version 410 core
void main(void)
{
    const vec4 vertices[] = vec4[](vec4( 0.25, -0.25, 0.5, 1.0),
                                   vec4(-0.25, -0.25, 0.5, 1.0),
                                   vec4( 0.25,  0.25, 0.5, 1.0));

    gl_Position = vertices[gl_VertexID];
}
)";

static const char tcsShaderSource[] = R"(
#version 410 core
layout (vertices = 3) out;

void main(void)
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = 5.0;
        gl_TessLevelOuter[0] = 5.0;
        gl_TessLevelOuter[1] = 5.0;
        gl_TessLevelOuter[2] = 5.0;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
)";

static const char tesShaderSource[] = R"(
#version 410 core
layout (triangles, equal_spacing, cw) in;

void main(void)
{
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +
                  (gl_TessCoord.y * gl_in[1].gl_Position) +
                  (gl_TessCoord.z * gl_in[2].gl_Position);
}
)";

const char geometryShaderSource[] = R"(
#version 410 core
layout (triangles) in;
layout (line_strip, max_vertices = 3) out;

void main(void)
{
    int i;

    for (i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
}
)";

const char fragmentShaderSource[] = R"(

#version 410 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture0;
void main()
{
	FragColor = vec4(0.0, 0.8, 1.0, 1.0);
}
)";

float quadVertices[] = {
    // Î»ÖÃ          // ÑÕÉ«
    1.0f,   1.0f, -0.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, -0.0f,  0.0f, 0.0f,
    -1.0f, -1.0f, -0.0f,  0.0f, 1.0f,
    1.0f,   1.0f, -0.0f,   1.0f, 0.0f,
    -1.0f, -1.0f, -0.0f,  0.0f, 1.0f,
    1.0f,  -1.0f, -0.0f,   1.0f, 1.0f
};

#endif // !SHADERS_H_
