//http://www.songho.ca/opengl/gl_pbo.html

#include "my_shaders.h"

#include <map>
#include <chrono>
#include <ctime>

class PixelBuffer : public GLRender {
public:
    PixelBuffer() = default;
    ~PixelBuffer() = default;
private:

    inline static size_t readRGB(uint8_t *buffer, size_t size) {
        static FILE *fp = fopen(R"(F:\videoFile\Apocalypse.rgb)", "rb");
        int ret = fread(buffer, 1, size, fp);
        if (ret < size) {
            rewind(fp);
            ret = fread(buffer, 1, size, fp);
        }
        return ret;
    }

    Shader ourShader;
    GLuint textureID0;
    int width = 1280, height = 720, nrComponents = 3;
    GLuint pbo_unpacks[2];
    GLuint pbo_packs[2];
    GLuint VAO, VBO;

    void drawLoopBefore() {
        glEnable(GL_DEPTH_TEST);

        // build and compile our shader zprogram
        // ------------------------------------
        ourShader = Shader(vertexShaderSource, fragmentShaderSource);

        glGenTextures(1, &textureID0);
        glBindTexture(GL_TEXTURE_2D, textureID0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINES);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenBuffersARB(2, pbo_unpacks);
        for (GLuint pbo_unpack : pbo_unpacks) {
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_unpack);
            glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, width * height * 3, nullptr, GL_STREAM_DRAW_ARB);
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
        }

        glGenBuffersARB(2, pbo_packs);
        for (GLuint pbo_pack : pbo_packs) {
            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pbo_pack);
            glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, width * height * 3, nullptr, GL_STREAM_READ_ARB);
            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
        }

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        ourShader.setInt("texture0", 0);
        ourShader.use();
    }

    int64_t start_time = getSystemTime();
    int64_t total_frames = 0;
    int64_t index = 0;
    int64_t nextIndex;
    void drawLoop() {
        index = (index + 1) % 2;
        nextIndex = (index + 1) % 2;

        ++total_frames;
        int64_t diff = getSystemTime() - start_time;
        printf("%lld  \n", total_frames * 1000000 / diff);
        if (diff > 5000000) {
            start_time = getSystemTime();
            total_frames = 0;
        }

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//         {
//             static uint8_t *buffer = new uint8_t[width * height * 3];
//             readRGB(buffer, width * height * 3);
// 
//             glBindTexture(GL_TEXTURE_2D, textureID0);
//             glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
//             glBindTexture(GL_TEXTURE_2D, 0);
//         }

         {
            glBindTexture(GL_TEXTURE_2D, textureID0);

            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_unpacks[index]);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo_unpacks[nextIndex]);
            uint8_t *buffer = (uint8_t*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
            if (buffer) {
                readRGB(buffer, width * height * 3);
                glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
            }
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
         }

        ourShader.use();
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        {
            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pbo_packs[index]);
            glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pbo_packs[nextIndex]);
            GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
            if (ptr) {
                static FILE *fp = fopen("../pbo.rgb", "wb");
                fwrite(ptr, 3, width * height, fp);
                rewind(fp);
                glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
            }
            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
        }

    }

    void drawLoopEnd() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void SetGLFWCallback() override{
        glfwSetFramebufferSizeCallback(m_window, this->framebuffer_size_callback);
    }
};

int main()
{
    GLRender *gl_ctx = new PixelBuffer();
    gl_ctx->runDraw();
    delete gl_ctx;
    return 0;
}