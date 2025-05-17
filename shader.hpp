#pragma once

#include <span>
#include <string>
#include <optional>



uint create_shader(std::string const& path, uint type);
uint create_program(std::span<uint> const& shaders);
