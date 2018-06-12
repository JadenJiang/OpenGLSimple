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

uniform float uTolerance;

const vec3 W = vec3( 0.2125, 0.7154, 0.0721 );

void main()
{
    vec4 irgb = texture2D(texture0, vec2(TexCoord.s, TexCoord.t));
    float luminance = dot(irgb.rgb, W);
	FragColor = mix(irgb, vec4(vec3(luminance), 1.0), uTolerance);
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
    vec4 SobelColor = vec4(sqrt(color_horizon * color_horizon + color_vertical * color_vertical), 1.0f);
    FragColor = mix(srcColor, SobelColor, intensity);
    //vec4 SobelColor = vec4(color_horizon, 1.0f);
    //FragColor = SobelColor;
}
)";


static const char emboss_filter[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;


void main()
{
    ivec2 ires = textureSize(texture0, 0 );
    float ResS = float(ires.s);
    float ResT = float(ires.t);

    vec2 stpp = vec2(1.0/ResS, 1.0/ResT);
    vec3 c00 = texture(texture0, TexCoord).rgb;
    vec3 cp1p1 = texture(texture0, TexCoord + stpp).rgb;
    
    vec3 diffs = c00 - cp1p1;
    float maxcolor = diffs.r;
    if(abs(diffs.g) > abs(maxcolor)) 
        maxcolor = diffs.g;
    if(abs(diffs.b) > abs(maxcolor)) 
        maxcolor = diffs.b;
    
    float gray = clamp(maxcolor + 0.5, 0, 1);
    vec3 color = vec3(gray, gray, gray);
    FragColor = vec4(color, 1);

}
)";

static const char toom_filter[] = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture0;

vec2 samplerSteps;
const int stride = 1;
uniform float uTolerance;
const float uQuantize = 1000.0f;

void main()
{
    ivec2 ires = textureSize(texture0, 0);
    float ResS = float(ires.s);
    float ResT = float(ires.t);
    samplerSteps = vec2(1.0f/ResS, 1.0f/ResT);


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
    for(int i = 0; i < 9; i++){
        sampleTex[i] = texture(texture0, TexCoord + offsets[i]).rgb;
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

   
    const vec3 w = vec3( 0.2125, 0.7154, 0.0721 );
    float h = dot(color_horizon, w);
    float v = dot(color_vertical, w);
    float mag = length(vec2(h, v));
    
    if(mag > uTolerance){
        FragColor = vec4(0, 0, 0, 1);
    }
    else{
        vec3 irgb = texture(texture0, TexCoord).rgb;
        //irgb.rgb *= uQuantize;
        //irgb.rgb += vec3(0.5);
        //ivec3 intrgb = ivec3(irgb.rgb);
        //irgb.rgb = vec3(intrgb)/uQuantize;
        FragColor = vec4(irgb.rgb, 1);
    }
}
)";



static const char kernel_filter[] = R"(
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

    float kernel[9] = float[](
        1, 1, 1,
        1,  -8, 1,
        1, 1, 1
    );
    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; ++i){
	    color += sampleTex[i] * kernel[i];
    }


    FragColor = vec4(color, 1.0f) + vec4(0.5, 0.5, 0.5, 1.0);

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
        //m_width_win = 900;
        //m_height_win = 1200;
    }
    ~EdgeFilter() = default;
private:
    Shader ourShader;
    GLuint VAO, VBO;
    GLuint floorTexture;

    float stride = 1;
    float intensity = 0.8;
    float uTolerance = 0.2;

    void drawLoopBefore() {
        glEnable(GL_DEPTH_TEST);

        //ourShader = Shader(s_vertexShader, s_fragShader);
        ourShader = Shader(s_vertexSobelShader, s_fragShader);
        floorTexture = loadTexture(R"(D:\videoFile\Lena.jpg)"); 
        //floorTexture = loadTexture(R"(F:\videoFile\Yoona.jpg)");


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


        ourShader.use();
        ourShader.setFloat("uTolerance", uTolerance);
        // floor
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
    }
    void keyEventCallBack(KeyType type) override {
        if (type == KEY_UP) {
            uTolerance += 0.003;
        }
        else if (type == KEY_DOWN) {
            uTolerance -= 0.003;
        }
        printf("%f\n", uTolerance);

    }
    void drawLoopEnd() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void SetGLFWCallback() {
        glfwSetFramebufferSizeCallback(m_window, this->framebuffer_size_callback);
    }
};

}