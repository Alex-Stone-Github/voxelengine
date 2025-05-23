#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvcord;

out vec2 texcord;

uniform mat4 transform;
uniform mat4 perspective;
uniform mat4 view;

void main() {
    texcord = uvcord;
    gl_Position = perspective * view * transform * vec4(position, 1.0);
}