#include "gl_render.h"
#include "stb_image.h"
#include <learnopengl/filesystem.h>

GLRender::GLRender() {
    m_window = nullptr;
    m_width_win = 1280;
    m_height_win = 720;

    // 	m_width = 900;
    // 	m_height = 1200;

    m_lastX = m_width_win / 2.0;
    m_lastY = m_height_win / 2.0;
    m_firstMouse = true;
    m_deltaTime = 0.0f;
    m_lastFrame = 0.0f;
}

void GLRender::runDraw() {
    createWindows();
    drawLoopBefore();
    while (!glfwWindowShouldClose(m_window))
    {
        processInput(m_window);
        m_view = m_camera.GetViewMatrix();
        m_projection = glm::perspective(glm::radians(m_camera.Zoom), (float)m_width_win / (float)m_height_win, 0.1f, 100.0f);

        drawLoop();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    drawLoopEnd();
    glfwTerminate();
}

void GLRender::createWindows() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

                                                         // glfw window creation
    m_window = glfwCreateWindow(m_width_win, m_height_win, "LearnOpenGL", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);

    SetGLFWCallback();

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}

void GLRender::SetGLFWCallback() {
    glfwSetFramebufferSizeCallback(m_window, this->framebuffer_size_callback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(m_window, this->mouse_callback);
    glfwSetScrollCallback(m_window, this->scroll_callback);
}

GLuint GLRender::loadTexture(const char *path) {
    GLuint textureID = -1;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    //stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINES);
        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void GLRender::processInput(GLFWwindow *window)
{
    float currentFrame = glfwGetTime();
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.ProcessKeyboard(FORWARD, m_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.ProcessKeyboard(BACKWARD, m_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.ProcessKeyboard(LEFT, m_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.ProcessKeyboard(RIGHT, m_deltaTime);
}

void GLRender::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos;
    m_lastX = xpos;
    m_lastY = ypos;
    m_camera.ProcessMouseMovement(xoffset, yoffset);
}

void GLRender::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    m_camera.ProcessMouseScroll(yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void GLRender::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

Camera GLRender::m_camera = glm::vec3(0.0f, 0.0f, 3.0f);
float GLRender::m_lastX = 0;
float GLRender::m_lastY = 0;
float GLRender::m_deltaTime = 0;
float GLRender::m_lastFrame = 0;
bool GLRender::m_firstMouse = true;