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
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	//gl_Position = projection * view *  model * vec4(aPos, 1.0);
	gl_Position = vec4(aPos, 1.0);
}
)";

const char fragmentShaderSource[] = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture;

void main()
{
	FragColor = texture(texture, TexCoord);
}
)";

const char Wave[] = R"(
#version 330 core
in vec2 TexCoord;
uniform sampler2D texture;

uniform float motion;
uniform float angle;
uniform float strength;
void main()
{
   vec2 coord;
   coord.x = TexCoord.x + strength * sin(motion + TexCoord.x * angle);
   coord.y = TexCoord.y + strength * sin(motion + TexCoord.y * angle);
   gl_FragColor = texture2D(texture, coord);
}
)";

const char Gray[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture;

uniform float motion;
uniform float angle;
uniform float strength;
void main()
{
	FragColor = texture(texture, TexCoord);
	float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	FragColor = vec4(average, average, average, 1.0);
}
)";


const char Kernel[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture;


const float offset = 1.0 / 300.0;
void main()
{
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0f, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(offset, 0.0f),
		vec2(-offset, -offset),
		vec2(0.0f, -offset),
		vec2(offset, -offset)
	);
	float kernel1[9] = float[](
		-1.0f, -1.0f, -1.0f,
		-1.0f,  9.0f, -1.0f,
		-1.0f, -1.0f, -1.0f
	);

	//Blur
	float kernel2[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	);

	//Edge-detection
	float kernel3[9] = float[](
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; ++i){
		sampleTex[i] = vec3(texture(texture, TexCoord.st + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; ++i){
		col += sampleTex[i] * kernel2[i];
	}
	FragColor = vec4(col, 1.0);
}
)";


float planeVertices[] = {
	// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	1.0f,   1.0f, -0.0f,  1.0f, 0.0f,
	-1.0f,  1.0f, -0.0f,  0.0f, 0.0f,
	-1.0f, -1.0f, -0.0f,  0.0f, 1.0f,
	1.0f,   1.0f, -0.0f,   1.0f, 0.0f,
	-1.0f, -1.0f, -0.0f,  0.0f, 1.0f,
	1.0f,  -1.0f, -0.0f,   1.0f, 1.0f
};




#endif // !SHADERS_H_

