#include "gl_render.h"

namespace Mosaic{

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

static const char mosaic_circle[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform vec2 winSize;
uniform vec2 mosaicSize;

void main()
{
    ivec2 screen_coord = ivec2(TexCoord.x * winSize.x, TexCoord.y * winSize.y);
    vec2 mosaic_central = vec2(   floor(screen_coord.x / mosaicSize.x) * mosaicSize.x,
                            floor(screen_coord.y / mosaicSize.y) * mosaicSize.y) + 0.5f * mosaicSize;
    vec2 delxy = mosaic_central - screen_coord;
    float delL = length(delxy);
    vec2 stMosaic = mosaic_central/winSize;

    vec4 finalColor;  
    if(delL < 0.5*mosaicSize.x){
        finalColor = texture(texture0, stMosaic);
    }
    else{
        finalColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        //finalColor = texture(texture0, TexCoord);
    }

    FragColor = finalColor;
}
)";



static const char mosaic_square[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform vec2 winSize;
uniform vec2 mosaicSize;

void main()
{
    ivec2 screen_coord = ivec2(TexCoord.x * winSize.x, TexCoord.y * winSize.y);
    vec2 mosaic_central;
	
	float offsetY = 0;
	if(mod(floor(screen_coord.x / mosaicSize.x), 2)== 0){
		mosaic_central = vec2(   floor(screen_coord.x / mosaicSize.x) * mosaicSize.x,
                            floor(screen_coord.y / mosaicSize.y) * mosaicSize.y) + 0.5f * mosaicSize;
	}
	else{
		mosaic_central = vec2(   floor(screen_coord.x / mosaicSize.x) * mosaicSize.x,
                            floor(screen_coord.y / mosaicSize.y) * mosaicSize.y) + 
							0.5f * mosaicSize + vec2(0, 0.5f * mosaicSize.y);
		if((mosaic_central.y - screen_coord.y) > mosaicSize.y * 0.5 ){
			mosaic_central = vec2(mosaic_central.x, mosaic_central.y - mosaicSize.y);
		}
		else if((mosaic_central.y - screen_coord.y) < -mosaicSize.y * 0.5){
			mosaic_central = vec2(mosaic_central.x, mosaic_central.y + mosaicSize.y);
		}
	}

    vec2 stMosaic = mosaic_central/winSize;

    vec4 finalColor;  
    if( abs(mosaic_central.x - screen_coord.x) <  mosaicSize.x * 0.5 &&
        abs(mosaic_central.y - screen_coord.y) <  mosaicSize.y * 0.5 ){
        finalColor = texture(texture0, stMosaic);
    }
    else{
        finalColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        //finalColor = texture(texture0, TexCoord);
    }

    FragColor = finalColor;
}
)";


static const char mosaic_hexagonal[] = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform vec2 winSize;
uniform vec2 mosaicSize;

void main()
{
    const float sqrt3 = sqrt(3);
	float len_x = mosaicSize.x;
	float len_y = mosaicSize.x * 0.5 * sqrt3;
    ivec2 screen_coord = ivec2(TexCoord.x * winSize.x, TexCoord.y * winSize.y);
	
	int wx = int(screen_coord.x/(1.5 * len_x));
	int wy = int(screen_coord.y/(len_y));
    vec2 v1, v2, mosaic_central;
    
    if(mod(wx, 2) == mod(wy, 2)){
        v1 = vec2(wx * len_x * 1.5, wy * len_y);
        v2 = vec2((wx + 1) * len_x * 1.5, (wy + 1) * len_y);
          
    }
    else{
        v1 = vec2(wx * len_x * 1.5, (wy + 1) * len_y);
        v2 = vec2((wx + 1) * len_x * 1.5, wy * len_y);
        FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    }
    
    float s1 = sqrt( pow(v1.x-screen_coord.x, 2) + pow(v1.y-screen_coord.y, 2) );  
    float s2 = sqrt( pow(v2.x-screen_coord.x, 2) + pow(v2.y-screen_coord.y, 2) );  

    if(s1 < s2){
        mosaic_central = v1;  
    }
    else{
        mosaic_central = v2;
    }
    vec2 stMosaic = mosaic_central/winSize;
    FragColor = texture(texture0, stMosaic);
   

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

class MosaicFilter : public GLRender {
public:
    MosaicFilter() = default;
    ~MosaicFilter() = default;
private:
    Shader ourShader;
    GLuint VAO, VBO;
    GLuint floorTexture;
    void drawLoopBefore() {
        ourShader = Shader(vertexShaderSource, mosaic_hexagonal);
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
        ourShader.setInt("texture0", 0);
        ourShader.setVec2("winSize", glm::vec2(m_width_win, m_height_win)); 
        ourShader.setVec2("mosaicSize", glm::vec2(6)); 
    }

    void drawLoop() {


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

	void SetGLFWCallback() override {
		glfwSetFramebufferSizeCallback(m_window, this->framebuffer_size_callback);
	}
};

}