#pragma once

#include <string>
#include <memory>

struct Image {
    unsigned int texture;
    int width, height;
    unsigned char* data;
    Image(std::string const& path);
};

void set_texture(unsigned int program, Image const& image);