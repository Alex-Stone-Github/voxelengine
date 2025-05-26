#pragma once

#include <string>
#include <memory>

struct Image {
    unsigned int texture;
    int width, height;
    unsigned int spot;
    unsigned char* data;
    Image(std::string const& path, unsigned int spot);
};

void set_texture(unsigned int program, Image const& image);