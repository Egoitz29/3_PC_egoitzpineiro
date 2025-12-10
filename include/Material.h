#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class Material {
public:
    Material();
    ~Material();

    bool load(const std::string& vsPath, const std::string& fsPath);

    void use() const;

    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setBool(const char* name, bool value) const;

    //  MATRIZ 4x4
    void setMat4(const char* name, const glm::mat4& value) const;



    void setVec3(const std::string& name, const glm::vec3& value);


private:
    GLuint program;

    std::string loadFile(const std::string& path);
    GLuint compileShader(GLenum type, const char* src);
};
