#pragma once
#include <glad/glad.h>
#include <string>

class Material {
public:
    Material();
    ~Material();

    bool load(const std::string& vsPath, const std::string& fsPath);

    void use() const;

    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setBool(const char* name, bool value) const;

    
    void setMat4(const char* name, const float* value) const;

private:
    GLuint program;

    std::string loadFile(const std::string& path);
    GLuint compileShader(GLenum type, const char* src);
};
