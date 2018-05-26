#ifndef SHADERS_H_
#define SHADERS_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define StringBuild(x) #x

static const char vertexShaderSource[] = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aOffset;

out vec3 fcolor;
//uniform vec2 offsets[100];


void main()
{
	//vec2 offset = offsets[gl_InstanceID];
	//gl_Position = vec4(aPos + offset, 0.0, 1.0);
	vec2 pos = aPos * (gl_InstanceID / 100.0);
	gl_Position = vec4(pos + aOffset, 0.0, 1.0);
	fcolor = aColor;
}
)";

const char fragmentShaderSource[] = R"(

#version 330 core
out vec4 FragColor;
in vec3 fcolor;
void main()
{
	FragColor = vec4(fcolor, 1.0);
}

)";




float quadVertices[] = {
	// Î»ÖÃ          // ÑÕÉ«
	-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
	0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
	-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

	-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
	0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
	0.05f,  0.05f,  0.0f, 1.0f, 1.0f
};


#endif // !SHADERS_H_

