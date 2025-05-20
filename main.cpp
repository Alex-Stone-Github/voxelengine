#include <print>
#include <cassert>
#include <string>
#include <algorithm>
#include <fstream>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "shader.hpp"

static constexpr int width = 900;
static constexpr int height = 600;

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
    float xbuffer[] = {s, -s, s};
    float ybuffer[] = {s, -s, -s};
    uint indices[] = {0, 1, 2};
    // Create Everything
    uint vao, xvbo, yvbo, ibo;
    glGenBuffers(1, &xvbo);
    glBindBuffer(GL_ARRAY_BUFFER, xvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xbuffer), xbuffer, GL_STATIC_DRAW);
    glGenBuffers(1, &yvbo);
    glBindBuffer(GL_ARRAY_BUFFER, yvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ybuffer), ybuffer, GL_STATIC_DRAW);
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glCreateVertexArrays(1, &vao);

    // Properties
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Make IBO Current
    glBindBuffer(GL_ARRAY_BUFFER, xvbo);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, yvbo);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);
    glEnableVertexAttribArray(1);

    bool running = true;
    while (running) {
        // Process Inputs
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }
        // Perform Updates and logic
        glViewport(0, 0, width, height);
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw Calls
        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // Upload Host buffer and transfere to device
        std::ranges::for_each(xbuffer, [](float& xv){
            xv += 0.001;
        });
        glBindBuffer(GL_ARRAY_BUFFER, xvbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(xbuffer), xbuffer, GL_STATIC_DRAW);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteProgram(program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &xvbo);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &yvbo);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}