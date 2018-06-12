#ifndef SHADERS_ALL_H
#define SHADERS_ALL_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shaders
{
private:
    GLuint vertex_obj_ = 0;
    GLuint tess_control_obj_  = 0;
    GLuint tess_evaluation_obj_ = 0;
    GLuint geometry_obj_ = 0;
    GLuint fragment_obj_ = 0;
    int loadShader(const char* shaderCode, GLenum type) {
        GLuint obj = 0;
        int success;
        char infoLog[512] = { 0 };
        obj = glCreateShader(type);
        glShaderSource(obj, 1, &shaderCode, NULL);
        glCompileShader(obj);
        return obj;
    }

    const char *getShaderFromFile(const char* path) {
        static std::string shaderCode;
        std::ifstream shaderFile;
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            if (strlen(path) > 50) {
                shaderCode = path;
            }
            else {
                // open files
                shaderFile.open(path);
                std::stringstream shaderStream;
                // read file's buffer contents into streams
                shaderStream << shaderFile.rdbuf();
                shaderFile.close();
                // convert stream into string
                shaderCode = shaderStream.str();
            }
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = shaderCode.c_str();
        return vShaderCode;
    }

public:
    GLuint ID = 0;
    Shaders() = default;

    int loadVertexShader(const char* shaderCode) {
        vertex_obj_ = loadShader(shaderCode, GL_VERTEX_SHADER);
        checkCompileErrors(vertex_obj_, "VERTEX_SHADER");
        return vertex_obj_;
    }
    int loadTessControlShader(const char* shaderCode) {
        tess_control_obj_ = loadShader(shaderCode, GL_TESS_CONTROL_SHADER);
        checkCompileErrors(tess_control_obj_, "TESS_CONTROL_SHADER");
        return tess_control_obj_;
    }
    int loadTessEvaluationShader(const char* shaderCode) {
        tess_evaluation_obj_ = loadShader(shaderCode, GL_TESS_EVALUATION_SHADER);
        checkCompileErrors(tess_evaluation_obj_, "TESS_EVALUATION_SHADER");
        return tess_evaluation_obj_;
    }
    int loadGeometryShader(const char* shaderCode) {
        geometry_obj_ = loadShader(shaderCode, GL_GEOMETRY_SHADER);
        checkCompileErrors(geometry_obj_, "GEOMETRY_SHADER");
        return geometry_obj_;
    }
    int loadFragmentShader(const char* shaderCode) {
        fragment_obj_ = loadShader(shaderCode, GL_FRAGMENT_SHADER);
        checkCompileErrors(fragment_obj_, "FRAGMENT_SHADER");
        return fragment_obj_;
    }

    int linkProgram(){
        ID = glCreateProgram();
        if (vertex_obj_ > 0) glAttachShader(ID, vertex_obj_);
        if (tess_control_obj_ > 0) glAttachShader(ID, tess_control_obj_);
        if (tess_evaluation_obj_ > 0) glAttachShader(ID, tess_evaluation_obj_);
        if (geometry_obj_ > 0) glAttachShader(ID, geometry_obj_);
        if (fragment_obj_ > 0) glAttachShader(ID, fragment_obj_);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        if (vertex_obj_ > 0) glDeleteShader(vertex_obj_);
        if (tess_control_obj_ > 0) glDeleteShader(tess_control_obj_);
        if (tess_evaluation_obj_ > 0) glDeleteShader(tess_evaluation_obj_);
        if (geometry_obj_ > 0) glDeleteShader(geometry_obj_);
        if (fragment_obj_ > 0) glDeleteShader(fragment_obj_);
        return ID;
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void useProgram()
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) 
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif