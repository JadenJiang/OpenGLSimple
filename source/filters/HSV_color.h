#include "gl_render.h"

namespace HSV {

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

static const char hsv[] = R"(
#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform float uT;

vec3 convertRGB2HSV(vec3 rgbColor)
{
    float h, s, v;
    float r = rgbColor.r;
    float g = rgbColor.g;
    float b = rgbColor.b;
    v = max(r, max(g, b));
    float maxval = v;
    float minval = min(r, min(g, b));
    if(maxval == 0)
        s = 0.0;
    else
        s = (maxval - minval) / maxval;

    if(s == 0.0){
        h = 0;
    }
    else{
        float delta = maxval - minval;
        if( r == maxval){
            h = (g - b) / delta;
        }
        else if(g == maxval){
            h = 2.0 + (b - r) / delta;
        }
        else if( b == maxval){
            h = 4.0 + (r - g) / delta;
        }
                    
        h *= 60;
        if( h < 0.0)
            h += 360;
        return vec3(h, s, v);
    }      
}

vec3 convertHSV2RGB(vec3 hsvcolor)
{
    float h = hsvcolor.x;
    float s = hsvcolor.y;
    float v = hsvcolor.z;
    if (s == 0.0) // achromatic– saturation is 0
    {
        return vec3(v, v, v);// return value as gray
    }
    else // chromatic case
    {
        if (h > 360.0) h = 360.0; // h must be in [0, 360)
        if (h < 0.0) h = 0.0; // h must be in [0, 360)
            h /= 60.0f;
        int k = int(h);
        float f = h - float(k);
        float p = v * (1.0 - s);
        float q = v * (1.0 - (s * f));
        float t = v * (1.0 - (s * (1.0 - f)));
        if (k == 0) return vec3(v, t, p);
        if (k == 1) return vec3(q, v, p);
        if (k == 2) return vec3(p, v, t);
        if (k == 3) return vec3(p, q, v);
        if (k == 4) return vec3(t, p, v);
        if (k == 5) return vec3(v, p, q);
    }
}



void main()
{
    vec4 color = texture(texture0, TexCoord);
    vec3 ihsv =  convertRGB2HSV(color.rgb);
    ihsv.x += uT;
    if(ihsv.x > 360)
        ihsv.x -= 360;
    if(ihsv.x < 0)
        ihsv.x += 360;
    color.rgb = convertHSV2RGB(ihsv);
    FragColor = color;
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

class HSVFilter : public GLRender {
public:
    HSVFilter() = default;
    ~HSVFilter() = default;
private:
    Shader ourShader;
    GLuint VAO, VBO;
    GLuint floorTexture;
    float  uT = 240;

    void drawLoopBefore() {
        ourShader = Shader(vertexShaderSource, hsv);
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
        
    }

    void drawLoop() {
        // render
        glClearColor(0.f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setFloat("uT", uT);

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

    void keyEventCallBack(KeyType type) override{
        if (type == KEY_UP) {
            uT += 1;
        }
        else if (type == KEY_DOWN) {
            uT -= 1;
        }
    
    }

    void SetGLFWCallback() {
        glfwSetFramebufferSizeCallback(m_window, this->framebuffer_size_callback);
    }
};

}