#include "my_shaders.h"

class Triangle : public GLRender {
public:
    Triangle() = default;
    ~Triangle() = default;

private:
    GLuint VAO[2];
    GLuint VBO[2];
    Shader ourShader;
    void drawLoopBefore() override {
        glEnable(GL_DEPTH_TEST);

        // build and compile our shader zprogram
        // ------------------------------------
        ourShader = Shader(vertexShaderSource, fragmentShaderSource);
        //GLint location = glGetAttribLocation(ourShader.ID, "aPos");

        glGenVertexArrays(2, VAO);
        glGenBuffers(2, VBO);
        glBindVertexArray(VAO[0]);

        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle) + sizeof(color), nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(firstTriangle), firstTriangle);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(firstTriangle), sizeof(color), color);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(firstTriangle)));
        glBindVertexArray(0);

        glBindVertexArray(VAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle) + sizeof(color), nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(secondTriangle), secondTriangle);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(secondTriangle), sizeof(color), color);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(secondTriangle)));
        glBindVertexArray(0);
    }

    void drawLoop() override {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void drawLoopEnd() override {
    }

    void SetGLFWCallback() override {
        glfwSetFramebufferSizeCallback(m_window, this->framebuffer_size_callback);
    }
};

int main()
{
    GLRender *gl_ctx = new Triangle();
    gl_ctx->runDraw();
    delete gl_ctx;
    return 0;
}