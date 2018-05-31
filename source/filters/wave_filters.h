#include "gl_render.h"

namespace Wave {

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

static const char fragmentShaderSource[] = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;

void main()
{
FragColor = texture(texture0, TexCoord);
}
)";

static const char Wave[] = R"(
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

static const char Gray[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture0;


void main()
{
FragColor = texture(texture0, TexCoord);
float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
FragColor = vec4(average, average, average, 1.0);
}
)";

static const char Kernel[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture0;

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
	sampleTex[i] = vec3(texture(texture0, TexCoord.st + offsets[i]));
}
vec3 col = vec3(0.0);
for(int i = 0; i < 9; ++i){
	col += sampleTex[i] * kernel2[i];
}
FragColor = vec4(col, 1.0);
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

class WaveFilter : public GLRender {
public:
    WaveFilter() :
        GLRender()
    {
    }
    ~WaveFilter() = default;
private:
    Shader ourShader;
    GLuint VAO, VBO;
    GLuint floorTexture;
    float m_motionSpeed = 3;
    float m_motion = 0;
    float m_angle = 40;
    void drawLoopBefore() {
        ourShader = Shader(vertexShaderSource, Wave);
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
        ourShader.setInt("texture", 0);	//每个着色器采样器属于哪个纹理单元
        ourShader.setFloat("angle", m_angle);
        ourShader.setFloat("strength", 0.003);
    }

    void drawLoop() {
        m_motion += m_motionSpeed * m_deltaTime;
        ourShader.setFloat("motion", m_motion);
        if (m_motion > 3.14159f * m_angle)
        {
            m_motion -= 3.14159f * m_angle;
        }

        // render
        glClearColor(0.f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 model, view, projection;
        ourShader.setMat4("projection", m_projection);

        // floor
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        ourShader.setMat4("model", glm::mat4());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    void drawLoopEnd() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

}