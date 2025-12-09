#include "Material.h"
#include <fstream>
#include <sstream>
#include <iostream>

Material::Material() : program(0) {}

Material::~Material() {
    if (program != 0)
        glDeleteProgram(program);
}

std::string Material::loadFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cout << "No se pudo abrir: " << path << "\n";
        return "";
    }

    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

GLuint Material::compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int ok;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

    if (!ok) {
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cout << "Error compilando shader:\n" << log << "\n";
    }

    return shader;
}

bool Material::load(const std::string& vsPath, const std::string& fsPath) {
    std::string vcode = loadFile(vsPath);
    std::string fcode = loadFile(fsPath);

    if (vcode.empty() || fcode.empty())
        return false;

    GLuint vs = compileShader(GL_VERTEX_SHADER, vcode.c_str());
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fcode.c_str());

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int ok;
    char log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &ok);

    if (!ok) {
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cout << "Error linkeando programa:\n" << log << "\n";
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return true;
}

void Material::use() const {
    glUseProgram(program);
}

void Material::setInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(program, name), value);
}

void Material::setFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(program, name), value);
}

void Material::setBool(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(program, name), value ? 1 : 0);
}

// ✅ NUEVO: ENVÍO DE MATRIZ 4x4 AL SHADER
void Material::setMat4(const char* name, const float* value) const {
    glUniformMatrix4fv(
        glGetUniformLocation(program, name),
        1,
        GL_FALSE,
        value
    );
}
