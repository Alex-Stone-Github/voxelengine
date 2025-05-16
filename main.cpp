#include <print>
#include <cassert>
#include <string>
#include <fstream>
#include <SDL2/SDL.h>
#include <GL/gl.h>

std::string read_file(std::string const& path) {
    static char buffer[512] = {0};
    std::string file_content;
    FILE* file = fopen(path.c_str(), "r");
    assert(file);
    size_t read_count = 0;
    do {
        memset(buffer, 0, sizeof(buffer));
        read_count = fread(buffer, 1, sizeof(buffer), file);
        std::string section(buffer, read_count);
        file_content += section;
    } while (read_count > 0);
    fclose(file);
    return file_content;
}

static constexpr int width = 900;
static constexpr int height = 600;

int main() {
    SDL_Window* window = SDL_CreateWindow("Opengl Application",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
        SDL_WINDOW_OPENGL);
    assert(window);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    // Testing
    auto cont = read_file("vert.glsl");
    std::println("{}", cont);

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
        SDL_GL_SwapWindow(window);
    }
    return 0;
}