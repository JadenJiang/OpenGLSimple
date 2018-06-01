#ifndef SHADERS_H_
#define SHADERS_H_

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
#include "gl_render.h"

#undef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


static const char vertexShaderSource[] = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
void main()
{
    gl_Position = vec4(aPos, 1.0);
	TexCoord = (aTexCoord.xy);
}

)";

const char fragmentShaderSource[] = R"(

#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture0;
void main()
{
	FragColor = texture2D(texture0, TexCoord);
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

