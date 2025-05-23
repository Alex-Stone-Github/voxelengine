#include <print>
#include <cassert>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "shader.hpp"
#include "camera.hpp"
#include "vector3.hpp"
#include "chunk.hpp"
#include "display.hpp"

int main() {
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
    SDL_Window* window = SDL_CreateWindow("Triangle",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_OPENGL);
    assert(window);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    assert(glcontext);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    assert(glewInit() == GLEW_OK);

    // Game Objects
    Camera camera(Vector3(0, 0, 0), 0, 0);
    Chunk chunk;
    recompute_mesh(chunk, chunk, chunk, chunk, chunk, chunk, chunk);

    // Shaders -------------------
    auto vert = create_shader("./shader/vert.glsl", GL_VERTEX_SHADER);
    auto frag = create_shader("./shader/frag.glsl", GL_FRAGMENT_SHADER);
    uint tmp[] = {vert, frag};
    auto program = create_program(tmp);
    glDeleteShader(vert);
    glDeleteShader(frag);

    bool running = true;
    while (running) {
        // Process Inputs
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_A) camera.yaw += 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_D) camera.yaw -= 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_W) camera.pitch += 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_S) camera.pitch -= 0.1;

                if (event.key.keysym.scancode == SDL_SCANCODE_I) camera.position.z -= 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_K) camera.position.z += 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_J) camera.position.x -= 0.1;
                if (event.key.keysym.scancode == SDL_SCANCODE_L) camera.position.x += 0.1;
            }
        }
        camera.clamp(); // Make sure we don't overextend the view angles

        // Clear Buffers before next rendering
        glViewport(0, 0, width, height);
        glClearColor(0.8, 0.8, 0.8, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);

        draw_chunk(chunk, camera, program);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteProgram(program);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}