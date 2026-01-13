#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* -------- CONFIG -------- */
#define WIDTH 1920
#define HEIGHT 1080
#define RADIUS 20
#define MAX_TARGETS 6
/* ------------------------ */

typedef enum {
    MODE_FLICK,
    MODE_TRACKING
} GameMode;

typedef struct {
    int x, y, r;
} Target;

typedef struct {
    float x, y;
    float speed;
    int r;
} MovingTarget;

/* -------- DRAW CIRCLE -------- */
void draw_circle(SDL_Renderer* r, int cx, int cy, int rad) {
    for (int w = -rad; w <= rad; w++) {
        for (int h = -rad; h <= rad; h++) {
            if (w*w + h*h <= rad*rad) {
                SDL_RenderDrawPoint(r, cx + w, cy + h);
            }
        }
    }
}

/* -------- HIT -------- */
int hit(int tx, int ty, int tr, int mx, int my) {
    int dx = mx - tx;
    int dy = my - ty;
    return dx*dx + dy*dy <= tr*tr;
}

/* -------- TARGETS -------- */
Target new_target() {
    Target t;
    t.r = RADIUS;
    t.x = rand() % (WIDTH - 2*t.r) + t.r;
    t.y = rand() % (HEIGHT - 2*t.r) + t.r;
    return t;
}

MovingTarget new_moving_target() {
    MovingTarget t;
    t.r = 10 + rand() % 30;
    t.x = rand() % (WIDTH - 2*t.r) + t.r;
    t.y = -(rand() % HEIGHT);
    t.speed = 2 + rand() % 6;
    return t;
}

/* ================= MAIN ================= */
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    srand((unsigned int)time(NULL));

    SDL_Window* window = SDL_CreateWindow(
        "Aim Trainer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    /* ---- Sens ---- */
    float sensitivity = 0.18f;

    /* ---- Cursor virtual ---- */
    float cursor_x = WIDTH / 2;
    float cursor_y = HEIGHT / 2;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    /* ---- Game state ---- */
    GameMode mode = MODE_FLICK;

    Target flick_target = new_target();

    MovingTarget tracking_targets[MAX_TARGETS];
    for (int i = 0; i < MAX_TARGETS; i++)
        tracking_targets[i] = new_moving_target();

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT)
                running = 0;

            /* ---- Mouse movement ---- */
            if (event.type == SDL_MOUSEMOTION) {
                cursor_x += event.motion.xrel * sensitivity;
                cursor_y += event.motion.yrel * sensitivity;

                if (cursor_x < 0) cursor_x = 0;
                if (cursor_y < 0) cursor_y = 0;
                if (cursor_x > WIDTH)  cursor_x = WIDTH;
                if (cursor_y > HEIGHT) cursor_y = HEIGHT;
            }

            /* ---- Shoot ---- */
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT) {

                int mx = (int)cursor_x;
                int my = (int)cursor_y;

                if (mode == MODE_FLICK) {
                    if (hit(flick_target.x, flick_target.y,
                            flick_target.r, mx, my)) {
                        flick_target = new_target();
                    }
                } else {
                    for (int i = 0; i < MAX_TARGETS; i++) {
                        if (hit((int)tracking_targets[i].x,
                                (int)tracking_targets[i].y,
                                tracking_targets[i].r,
                                mx, my)) {
                            tracking_targets[i] = new_moving_target();
                        }
                    }
                }
            }

            /* ---- Keys ---- */
            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;

                if (event.key.keysym.sym == SDLK_1)
                    mode = MODE_FLICK;

                if (event.key.keysym.sym == SDLK_2)
                    mode = MODE_TRACKING;
            }
        }

        /* ---- Update tracking ---- */
        if (mode == MODE_TRACKING) {
            for (int i = 0; i < MAX_TARGETS; i++) {
                tracking_targets[i].y += tracking_targets[i].speed;
                if (tracking_targets[i].y - tracking_targets[i].r > HEIGHT)
                    tracking_targets[i] = new_moving_target();
            }
        }

        /* ---- Render ---- */
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        if (mode == MODE_FLICK)
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
        else
            SDL_SetRenderDrawColor(renderer, 80, 180, 255, 255);

        if (mode == MODE_FLICK) {
            draw_circle(renderer,
                flick_target.x,
                flick_target.y,
                flick_target.r);
        } else {
            for (int i = 0; i < MAX_TARGETS; i++) {
                draw_circle(renderer,
                    (int)tracking_targets[i].x,
                    (int)tracking_targets[i].y,
                    tracking_targets[i].r);
            }
        }

        /* ---- Crosshair ---- */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, cursor_x - 6, cursor_y,
                                      cursor_x + 6, cursor_y);
        SDL_RenderDrawLine(renderer, cursor_x, cursor_y - 6,
                                      cursor_x, cursor_y + 6);

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();
    return 0;
}
