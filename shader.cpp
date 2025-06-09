#include "shader.hpp"

#include <algorithm>
#include <string>
#include <cstring>
#include <cassert>
#include <span>
#include <memory>
#include <optional> 
#include <iostream>
#include <GL/glew.h>

std::string read_file(std::string const& path) {
    char buffer[512] = {0};
    std::string file_content;
    FILE* file = fopen(path.c_str(), "r");
    assert(file);
    size_t read_count = 0;
    do {
        memset(buffer, 0, sizeof(buffer));
        read_count = fread(buffer, 1, sizeof(buffer), file);
        std::string section(buffer, read_count);
        file_content += section;
    } while (read_count > 0);
    fclose(file);
    return file_content;
}
unsigned int create_shader(std::string const& path, unsigned int type) {
    auto const text = read_file(path);
    unsigned int shader = glCreateShader(type);
    char const* cont = text.data();
    int len = text.length();
    glShaderSource(shader, 1, &cont, &len);
    glCompileShader(shader);

    // Check for compile status
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        constexpr size_t length = 512;
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(length);
        glGetShaderInfoLog(shader, length, NULL, buffer.get());
        std::cout << "Shader Failure: " << std::string(buffer.get(), length);
        exit(1); // panic
    }
    return shader;
}
unsigned int create_program(std::span<unsigned int> const& shaders) {
    unsigned int program = glCreateProgram();
    std::ranges::for_each(shaders, [=](unsigned int shader){
        glAttachShader(program, shader);
    });
    glLinkProgram(program);
    // Check for link status
    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        constexpr size_t length = 512;
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(length);
        glGetProgramInfoLog(program, length, NULL, buffer.get());
        std::cout << "Shader Failure: " << std::string(buffer.get(), length);
        exit(1); // panic
    }
    return program;
}
