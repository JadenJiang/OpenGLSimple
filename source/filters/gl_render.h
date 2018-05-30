#ifndef GL_RENDER_H_
#define GL_RENDER_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <iostream>
#include <functional>

class GLRender {
public:
    GLRender();
    virtual ~GLRender() = default;
    void runDraw();
private:
    void createWindows();
    virtual void drawLoopBefore() { }
    virtual void drawLoop() { }
    virtual void drawLoopEnd() { }

private:

    GLFWwindow* m_window;
protected:
    int m_width;
    int m_height;
    glm::mat4 m_view;
    glm::mat4 m_projection;

    static Camera m_camera;
    static float m_lastX;
    static float m_lastY;
    static float m_deltaTime;
    static float m_lastFrame;
    static bool m_firstMouse;

protected:

    GLuint loadTexture(const char *path);

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    static void processInput(GLFWwindow *window);

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};

#endif // !SHADERS_H_
