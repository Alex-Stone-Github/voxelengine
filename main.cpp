#include <print>
#include <cassert>
#include <string>
#include <memory>
#include <algorithm>
#include <cmath>
#include <fstream>
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
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Game Objects
    Camera camera(Vector3(0, 32.0, 0), 0, 0);
    IndexId aprxpos(0, 0, 0);
    World world;
    Gizmo entity(gposition, gtexture, 4, Vector3(0, 32, -10));

    // Shaders -------------------
    auto vert = create_shader("./shader/vert.glsl", GL_VERTEX_SHADER);
    auto frag = create_shader("./shader/frag.glsl", GL_FRAGMENT_SHADER);
    uint tmp[] = {vert, frag};
    auto program = create_program(tmp);
    glDeleteShader(vert);
    glDeleteShader(frag);
    // Textures
    Image etexture("./picture/image.png", 0);
    Image atlas("./picture/atlas.png", 1);

    // Network
    std::println("Network Initialization Successful: {}", net::init());
    net::ClientGetChunkUpdate(IndexId(69, 70, 80));
    // Thread Spinning
    std::thread netthread(net::spinup);

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
            camera.position.z -= cos(camera.yaw+M_PI_2)*.1;
            camera.position.x -= sin(camera.yaw+M_PI_2)*.1;
        }
        if (keys_down.contains(SDL_SCANCODE_D)) {
            camera.position.z -= cos(camera.yaw-M_PI_2)*.1;
            camera.position.x -= sin(camera.yaw-M_PI_2)*.1;
        }
        if (keys_down.contains(SDL_SCANCODE_SPACE)) camera.position.y += 0.1;
        if (keys_down.contains(SDL_SCANCODE_LSHIFT)) camera.position.y -= 0.1;

        if (keys_down.contains(SDL_SCANCODE_L)) {
            net::ClientGetChunkFull(IndexId(4, 5, 6));
        }

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
        draw_gizmo(entity, camera, program);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    glDeleteProgram(program);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}