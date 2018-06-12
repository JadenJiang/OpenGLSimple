//http://www.songho.ca/opengl/gl_pbo.html

#include "my_shaders.h"

#include <map>
#include <chrono>
#include <ctime>

class GeometryShader : public GLRender {
public:
    GeometryShader() = default;
    ~GeometryShader() = default;
private:
    Shaders shader;
    GLuint vao;
    void drawLoopBefore() {
        glEnable(GL_DEPTH_TEST);
        shader.loadVertexShader(vertexShaderSource);
        shader.loadTessControlShader(tcsShaderSource);
        shader.loadTessEvaluationShader(tesShaderSource);
        shader.loadGeometryShader(geometryShaderSource);
        shader.loadFragmentShader(fragmentShaderSource);
        shader.linkProgram();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        shader.useProgram();
    }

    void drawLoop() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader.ID);

        glPointSize(5.0f);

        glDrawArrays(GL_PATCHES, 0, 3);
    }

    void drawLoopEnd() {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(shader.ID);
    }

    void SetGLFWCallback() override {
        glfwSetFramebufferSizeCallback(m_window, this->framebuffer_size_callback);
    }
};

int main()
{
    GLRender *gl_ctx = new GeometryShader();
    gl_ctx->runDraw();
    delete gl_ctx;
    return 0;
}