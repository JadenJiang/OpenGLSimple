#include "gl_render.h"

#define StringBuild(x) #x

static const char vertexShaderSource[] = R"(
#version 330 core
layout (location = 0) in vec3 aPos;		//Vertex Attribute
layout (location = 1) in vec3 aColor;
out vec4 color;
void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	color = vec4(aColor, 1.0f);
}
)";

const char fragmentShaderSource[] = R"(
#version 330 core
out vec4 FragColor;
in vec4 color;
void main()
{
	//FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	FragColor = color;
}
)";

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float firstTriangle[] = {
    -0.9f, -0.5f, 0.0f,  // left
    -0.0f, -0.5f, 0.0f,  // right
    -0.45f, 0.5f, 0.0f,  // top
};
float secondTriangle[] = {
    0.0f, -0.5f, 0.0f,  // left
    0.9f, -0.5f, 0.0f,  // right
    0.45f, 0.5f, 0.0f   // top
};
float color[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
};