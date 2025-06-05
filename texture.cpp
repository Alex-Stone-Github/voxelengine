#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "vender/stb/stb_image.h"

#ifdef WIN
#include "GL/glew.h"
#else
#include <GL/glew.h>
#endif

Image::Image(std::string const& path, unsigned int spot): spot(spot) {
    int throwit;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(path.c_str(), &width, &height, &throwit, 3);
    assert(data);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0+spot);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}
void set_texture(unsigned int program, Image const& image) {
    glActiveTexture(GL_TEXTURE0+image.spot);
    glBindTexture(GL_TEXTURE_2D, image.texture);
    glUseProgram(program);
    auto location = glGetUniformLocation(program, "tex");
    glUniform1i(location, image.spot); // Shader Location 0
}