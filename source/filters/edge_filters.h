#include "gl_render.h"

namespace Edge{

static const char s_vertexShader[] = R"(
#version 330 core
layout(location = 0)in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
void main()
{
    gl_Position = vec4(aPos, 1.0);
	TexCoord = (aTexCoord.xy);
}

)";

static const char s_fragShader[] = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;
uniform vec2 samplerSteps;
uniform float stride;
uniform float intensity;
uniform vec2 norm;

void main()
{
    vec4 src = texture2D(texture0, TexCoord);
    vec3 tmpColor = texture2D(texture0, TexCoord + samplerSteps * stride * norm).rgb;
    tmpColor = abs(src.rgb - tmpColor) * 2.0;
	FragColor = vec4(mix(src.rgb, tmpColor, intensity), src.a);
}
)";



static const char s_vertexSobelShader[] = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 samplerSteps;
uniform float stride;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = vec4(aPos, 1.0);
	TexCoord = (aTexCoord.xy);
}
)";


static const char s_fragSobelShader[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform vec2 samplerSteps;
uniform float stride;
uniform float intensity;

void main()
{

    vec2 offsets[9] = vec2[](
	    vec2(-samplerSteps.x * stride, samplerSteps.y * stride),
	    vec2(0.0f                    , samplerSteps.y * stride),
	    vec2(samplerSteps.x * stride,  samplerSteps.y * stride),
	    vec2(-samplerSteps.x * stride, 0.0f),
	    vec2(0.0f, 0.0f),
	    vec2(samplerSteps.x * stride,  0.0f),
	    vec2(-samplerSteps.x * stride, -samplerSteps.y * stride),
	    vec2(0.0f, -samplerSteps.y * stride),
	    vec2(samplerSteps.x * stride, -samplerSteps.y * stride)
    );

    vec3 sampleTex[9];
	for(int i = 0; i < 9; ++i)
	{
		sampleTex[i] = texture2D(texture0, TexCoord + offsets[i]).rgb;
	}

    float kernel_horizon[9] = float[](
	    1, 2, 1,
	    0, 0, 0,
	    -1, -2, -1
    );
    vec3 color_horizon = vec3(0.0);
    for(int i = 0; i < 9; ++i){
	    color_horizon += sampleTex[i] * kernel_horizon[i];
    }

    float kernel_vertical[9] = float[](
	    -1, 0, 1,
	    -2, 0, 2,
	    -1, 0, 1
    );
    vec3 color_vertical = vec3(0.0);
    for(int i = 0; i < 9; ++i){
        color_vertical += sampleTex[i] * kernel_vertical[i];
    }

	vec4 srcColor = texture2D(texture0, TexCoord);

	FragColor = vec4(mix(srcColor.rgb, sqrt(color_horizon * color_horizon + color_vertical * color_vertical), intensity), srcColor.a);
}
)";



static float planeVertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
    1.0f,   1.0f, -0.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, -0.0f,  0.0f, 0.0f,
    -1.0f, -1.0f, -0.0f,  0.0f, 1.0f,
    1.0f,   1.0f, -0.0f,   1.0f, 0.0f,
    -1.0f, -1.0f, -0.0f,  0.0f, 1.0f,
    1.0f,  -1.0f, -0.0f,   1.0f, 1.0f
};

class EdgeFilter : public GLRender {
public:
    EdgeFilter() :
        GLRender()
    {
    }
    ~EdgeFilter() = default;
private:
    Shader ourShader;
    GLuint VAO, VBO;
    GLuint floorTexture;

    float stride = 1;
    float intensity = 0.8;

    void drawLoopBefore() {
        glEnable(GL_DEPTH_TEST);

        //ourShader = Shader(s_vertexShader, s_fragShader);
        ourShader = Shader(s_vertexSobelShader, s_fragSobelShader);
        floorTexture = loadTexture(R"(D:\videoFile\Lena.jpg)");

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);

        ourShader.use();
        ourShader.setFloat("stride", stride);
        ourShader.setFloat("intensity", intensity);
        ourShader.setVec2("samplerSteps", glm::vec2(1.0f / (m_width_win * 1), 1.0f / (m_height_win * 1)));
    }

    void drawLoop() {
  
        // render
        glClearColor(0.f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // floor
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
    }

    void drawLoopEnd() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

}