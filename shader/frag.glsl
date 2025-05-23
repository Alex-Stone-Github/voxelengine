#version 330 core

in vec2 texcord;

void main() {
    gl_FragColor = vec4(1.0*texcord.y, 0.2, 0.0, 1.0);
}