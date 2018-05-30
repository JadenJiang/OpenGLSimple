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
out vec2 coords[8];

uniform vec2 samplerSteps;
uniform float stride;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = vec4(aPos, 1.0);
	TexCoord = (aTexCoord.xy);
	coords[0] = TexCoord - samplerSteps * stride;
	coords[1] = TexCoord + vec2(0.0, -samplerSteps.y) * stride;
	coords[2] = TexCoord + vec2(samplerSteps.x, -samplerSteps.y) * stride;

	coords[3] = TexCoord - vec2(samplerSteps.x, 0.0) * stride;
	coords[4] = TexCoord + vec2(samplerSteps.x, 0.0) * stride;

	coords[5] = TexCoord + vec2(-samplerSteps.x, samplerSteps.y) * stride;
	coords[6] = TexCoord + vec2(0.0, samplerSteps.y) * stride;
	coords[7] = TexCoord + vec2(samplerSteps.x, samplerSteps.y) * stride;
	
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

varying vec2 coords[8];

void main()
{
	vec3 colors[8];

	for(int i = 0; i < 8; ++i)
	{
		colors[i] = texture2D(texture0, coords[i]).rgb;
	}

	vec4 src = texture2D(texture0, TexCoord);

	vec3 h = -colors[0] - 2.0 * colors[1] - colors[2] + colors[5] + 2.0 * colors[6] + colors[7];
	vec3 v = -colors[0] + colors[2] - 2.0 * colors[3] + 2.0 * colors[4] - colors[5] + colors[7];

	FragColor = vec4(mix(src.rgb, sqrt(h * h + v * v), intensity), 1.0);
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

    float stride = 2;
    float intensity = 1;
    glm::vec2 norm;


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
        //norm = glm::rotate(glm::vec2(1.0f, 1.0f), 3.14f * 3 / 4);
        ourShader.setFloat("stride", stride);
        ourShader.setFloat("intensity", intensity);
        norm  = glm::mat2(glm::rotate(glm::mat4(), glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f))) * glm::vec2(1.0f, 0.0f);
        ourShader.setVec2("norm", norm);
        ourShader.setVec2("samplerSteps", glm::vec2(1.0f / (m_width * 1), 1.0f / (m_height * 1)));
        
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