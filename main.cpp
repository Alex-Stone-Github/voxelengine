#include <print>
#include <cassert>
#include <string>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "shader.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

static constexpr int width = 900*2;
static constexpr int height = 600*2;

int main() {
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
    SDL_Window* window = SDL_CreateWindow("Triangle",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_OPENGL);
    assert(window);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    assert(glcontext);
    // 330 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // GLEW Extension Loader
    assert(glewInit() == GLEW_OK);

    // Shaders -------------------
    auto vert = create_shader("./vert.glsl", GL_VERTEX_SHADER);
    auto frag = create_shader("./frag.glsl", GL_FRAGMENT_SHADER);
    uint tmp[] = {vert, frag};
    auto program = create_program(tmp);


    // Buffers -------------------
    constexpr float s = 0.5;
    float buffer[] = {
        s, s, 0.0,
        -s, s, 0.0,
        -s, -s, 0.0,

        -s, -s, -0.8,
        s, -s, -0.8,
        s, s, -0.8,
    };
    uint indices[] = {0, 1, 2,     3, 4, 5};
    // Create Everything
    uint vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    static float up_rot_angle;
    static float vert_rot_angle;

    bool running = true;
    while (running) {
        // Process Inputs
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_A) up_rot_angle += 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_D) up_rot_angle -= 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_W) vert_rot_angle += 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_S) vert_rot_angle -= 0.1;
            }
        }

        // Perform Updates and logic
        glViewport(0, 0, width, height);
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Shader Uniforms
        glUseProgram(program);
        glm::mat4 view = glm::perspective(static_cast<float>(35.0/3.1415*180.0),
            static_cast<float>(width / height), 0.1f, 100.0f);
        glm::mat4 translate = glm::translate(glm::vec3(0, 0, -1.5));
        auto xlook = -sin(up_rot_angle);
        auto ylook = sin(vert_rot_angle);
        auto zlook = -cos(up_rot_angle);
        glm::mat4 camera = glm::lookAt(
            glm::vec3(0, 0, 0), glm::vec3(xlook, ylook, zlook), glm::vec3(0, 1, 0));
        glm::mat4 transform = view * camera * translate;
        auto location = glGetUniformLocation(program, "transform");
        glUniformMatrix4fv(location, 1, GL_FALSE, &transform[0][0]);

        // Draw Calls
        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteProgram(program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &vbo);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}