#define SDL_MAIN_HANDLED
#include "sdlUtils.h"

int   initialize();
int   mainLoop();
void  updateDisplay();
int   checkResize(SDL_Event);
int   checkQuits(SDL_Event);

#define WIDTH (1080)
#define HEIGHT (720)


//begin globals 
SDL_Window    *window;
SDL_Renderer  *renderer;
SDL_Surface   *surface;
//end globals

int main(int argc, char **argv) {
    if(initialize()) { return 1; }
    updateDisplay();
    while(mainLoop()) {}
    SDL_Quit();
    return 0;
}

int initialize() {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed. Error code: %d", SDL_GetError());
        return 1;
    }
    
    window = SDL_CreateWindow("Utils Test",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    if(!window) {
        SDL_Log("Window creation failed. Error code: %d", SDL_GetError());
        return 1;
    }
    surface = SDL_GetWindowSurface(window);

    renderer = SDL_CreateSoftwareRenderer(surface);
    if(!renderer) {
        SDL_Log("Renderer creation failed. Error code: %d", SDL_GetError());
        return 1;
    }
    
    return 0;
}

int mainLoop() {
    static SDL_Event e;
    static int redraw = 1;
    static int i;

    redraw = 0;
    while(SDL_PollEvent(&e)) {

        i = checkResize(e);
        if(i) {
            if(i < 0) {
                return 0;
            }
            redraw = 1;
        }

        if(checkQuits(e)) { return 0; }
    }

    if(redraw) {
        updateDisplay();
    }

    return 1;
}

void updateDisplay() {
    static SDL_FPoint p;
    static float r;

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    if(SDL_RenderClear(renderer)) {
        SDL_Log("Rendering clear failed. Error: %s", SDL_GetError());
    }
    
    p.x = 400;
    p.y = 300;
    r = 100;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDLUtils_RenderDrawCircleF(renderer, p, r, 50);

    p.x += (2 * r) + 5;
    SDLUtils_RenderFillCircleF(renderer, p, r, 2);
    
    if(SDL_UpdateWindowSurface(window)) {
        SDL_Log("Surface update failed. Error: %s", SDL_GetError());
    }
}


int checkResize(SDL_Event e) {
    if((e.type == SDL_WINDOWEVENT) && (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
        SDL_DestroyRenderer(renderer);
        surface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateSoftwareRenderer(surface);
        if(!renderer) {
            SDL_Log("Renderer creation failed. Error: %s", SDL_GetError());
            return -1;
        }
        return 1;
    }
    return 0;
}

int checkQuits(SDL_Event e) {
    if(e.type == SDL_QUIT) {
        return 1;
    }
    if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        return 1;
    }
    return 0;
}


