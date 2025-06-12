#include <cassert>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <thread>

#include "shader.hpp"
#include "camera.hpp"
#include "vector3.hpp"
#include "chunk.hpp"
#include "display.hpp"
#include "texture.hpp"
#include "world.hpp"
#include "gizmo.hpp"
#include "network.hpp"

static constexpr auto HALF_PI = 3.1415 / 2.0;
static Vector3 const gposition[] = {
    {1, 1, 0},
    {-1, 1, 0},
    {-1, -1, 0},
    {1, -1, 0},
};
static Vector2 const gtexture[] = {
    {1, 1},
    {0, 1},
    {0, 0},
    {1, 0},
};

int main() {
    std::cout << "Starting Initialization!" << std::endl;
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
    SDL_Window* window = SDL_CreateWindow("Triangle",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_OPENGL);
    assert(window);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glcontext);
    assert(glcontext);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    glewExperimental = GL_TRUE;
    auto status = glewInit();
    assert(status == GLEW_OK);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    std::cout << "opengl version: " << glGetString(GL_VERSION) << std::endl;


    // Game Objects
    Camera camera(Vector3(0, 32.0, 0), 0, 0);
    IndexId aprxpos(0, 0, 0);
    World world;
    world.lcchunk.insert(std::pair(IndexId(0, 0, 0), generate(IndexId(0, 0, 0))));
    world.lcchunk.insert(std::pair(IndexId(0, 0, -1), generate(IndexId(0, 0, -1))));
    world.lcchunk.insert(std::pair(IndexId(1, 0, -1), generate(IndexId(1, 0, -1))));
    world.lcchunk.insert(std::pair(IndexId(2, 0, -2), generate(IndexId(2, 0, -2))));
    world.entities.emplace_back(Gizmo(gposition, gtexture, 4, Vector3(0, 32, -10)));


    // Shaders -------------------
    auto vert = create_shader("./shader/vert.glsl", GL_VERTEX_SHADER);
    auto frag = create_shader("./shader/frag.glsl", GL_FRAGMENT_SHADER);
    unsigned int tmp[] = {vert, frag};
    auto program = create_program(tmp);
    glDeleteShader(vert);
    glDeleteShader(frag);
    // Textures
    Image etexture("./picture/image.png", 0);
    Image atlas("./picture/atlas.png", 1);

    std::cout << "Connecting to the network!" << std::endl;
    // Network
    std::cout << "Network Initialization Successful: " << net::pl_open() << std::endl;
    net::ClientGetChunkUpdate(IndexId(69, 70, 80));
    // Thread Spinning
    std::thread netthread(net::spinup, &world);

    // SDL
    bool running = true;
    std::set<int> keys_down;
    while (running) {
        // Process Inputs
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) keys_down.insert(event.key.keysym.scancode);
            if (event.type == SDL_KEYUP) keys_down.erase(event.key.keysym.scancode);
            if (event.type == SDL_MOUSEMOTION) {
                auto dx = event.motion.xrel * 0.005;
                auto dy = -event.motion.yrel * 0.005;
                camera.yaw -= dx;
                camera.pitch += dy;
            }
        }
        camera.clamp(); // Make sure we don't overextend the view angles

        // Key Input
        if (keys_down.contains(SDL_SCANCODE_ESCAPE)) running = false;
        if (keys_down.contains(SDL_SCANCODE_W)) {
            camera.position.z -= cos(camera.yaw)*.2;
            camera.position.x -= sin(camera.yaw)*.2;
        }
        if (keys_down.contains(SDL_SCANCODE_S)) {
            camera.position.z += cos(camera.yaw)*.1;
            camera.position.x += sin(camera.yaw)*.1;
        }
        if (keys_down.contains(SDL_SCANCODE_A)) {
            camera.position.z -= cos(camera.yaw+HALF_PI)*.1;
            camera.position.x -= sin(camera.yaw+HALF_PI)*.1;
        }
        if (keys_down.contains(SDL_SCANCODE_D)) {
            camera.position.z -= cos(camera.yaw-HALF_PI)*.1;
            camera.position.x -= sin(camera.yaw-HALF_PI)*.1;
        }
        if (keys_down.contains(SDL_SCANCODE_SPACE)) camera.position.y += 0.1;
        if (keys_down.contains(SDL_SCANCODE_LSHIFT)) camera.position.y -= 0.1;

        // Clear Buffers before next rendering
        glViewport(0, 0, width, height);
        glClearColor(0.8, 0.8, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);

        // Process the chunks - refresh meshes - etc
        auto currentx_aprx = static_cast<int>(camera.position.x / (sizex * blocksize));
        auto currentz_aprx = static_cast<int>(camera.position.z / (sizez * blocksize));
        aprxpos = IndexId(currentx_aprx, 0, currentz_aprx);
        world.reload_check(aprxpos);
        world.dirty_check();

        // Draw the chunks
        set_texture(program, atlas);
        std::ranges::for_each(world.live_chunks, [&](LiveChunk const& chunk) {
            draw_chunk(chunk, camera, program);
        });
        set_texture(program, etexture);
        std::ranges::for_each(world.entities, [&](Gizmo const& entity) {
            draw_gizmo(entity, camera, program);
        });

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    net::pl_close();
    glDeleteProgram(program);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}