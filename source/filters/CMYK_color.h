#include "gl_render.h"

namespace CMYK {

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

static const char grayscale[] = R"(
#version 330 core

#define CYAN
//#undef CYAN
//#undef MAGENTA
//#undef YELLOW
//#undef BLACK

out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture0;


void main()
{
    vec4 color = texture(texture0, TexCoord);

    vec3 cmycolor = vec3(1.0) - color.rgb;
    float k = min(cmycolor.r, min(cmycolor.g, cmycolor.b));
    vec3 temp = cmycolor - 0.1 * k;
    if( k < 0.3)
        k = 0;
    else
        k = 0.9 * (k - 0.3)/0.7;
    vec4 cmykcolor = vec4(temp, k);

#ifdef CYAN
    FragColor = vec4(vec3(1.0 - cmykcolor.r), 1);
#endif

#ifdef MAGENTA
    FragColor = vec4(vec3(1.0 - cmykcolor.g), 1);
#endif

#ifdef YELLOW
    FragColor = vec4(vec3(1.0 - cmykcolor.b), 1);
#endif

#ifdef BLACK
    FragColor = vec4(vec3(1.0 - cmykcolor.a), 1);
#endif

    //float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    //FragColor = vec4(vec3(average), 1);
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

class CMYKFilter : public GLRender {
public:
    CMYKFilter() = default;
    ~CMYKFilter() = default;
private:
    Shader ourShader;
    GLuint VAO, VBO;
    GLuint floorTexture;

    void drawLoopBefore() {
        ourShader = Shader(vertexShaderSource, grayscale);
        floorTexture = loadTexture(R"(F:\videoFile\Lena.jpg)");

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
    }

    void drawLoop() {
        // render
        glClearColor(0.f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        // floor
        glBindVertexArray(VAO);
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