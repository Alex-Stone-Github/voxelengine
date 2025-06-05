#pragma once

#include <span>
#include <string>
#include <optional>



unsigned int create_shader(std::string const& path, unsigned int type);
unsigned int create_program(std::span<unsigned int> const& shaders);