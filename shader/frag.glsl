#version 330 core

in vec2 texcord;
uniform sampler2D tex;

void main() {
    vec4 color = texture(tex, texcord);
    gl_FragColor = color;
}