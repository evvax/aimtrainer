#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define RADIUS 20

typedef struct {
    int x;
    int y;
    int r;
} Target;

void draw_circle(SDL_Renderer* renderer, Target t) {
    for (int w = -t.r; w <= t.r; w++) {
        for (int h = -t.r; h <= t.r; h++) {
            if (w*w + h*h <= t.r * t.r) {
                SDL_RenderDrawPoint(renderer, t.x + w, t.y + h);
            }
        }
    }
}

int hit_target(Target t, int mx, int my) {
    int dx = mx - t.x;
    int dy = my - t.y;
    return (dx*dx + dy*dy) <= t.r * t.r;
}

Target new_target() {
    Target t;
    t.r = RADIUS;
    t.x = rand() % (WIDTH - 2*t.r) + t.r;
    t.y = rand() % (HEIGHT - 2*t.r) + t.r;
    return t;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL));

    SDL_Window* window = SDL_CreateWindow(
        "Aim Trainer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Target target = new_target();
    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {

                int mx = event.button.x;
                int my = event.button.y;

                if (hit_target(target, mx, my)) {
                    target = new_target();
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
        draw_circle(renderer, target);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
