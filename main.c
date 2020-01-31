#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#define ROWS 25
#define COLS 35
#define BORDER 1
#define CELL 25
#define SIZE (2 * BORDER + CELL)
#define WIDTH (COLS * SIZE)
#define HEIGHT (ROWS * SIZE + 20)
#define FPS 30

unsigned char game_state[ROWS][COLS];
int countNeighbours(int x, int y);
void update();
void draw(SDL_Renderer* renderer);

int main() {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            game_state[i][j] = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
        printf("error initializing sdl");
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("error initializing sdl_image");
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("error initializing sdl_mixer");
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Azad, Conway's Game of Life",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* img_surface = IMG_Load("bottom_txt.png");
    if (!img_surface) {
        printf("error loading bottom_txt.png");
        return 1;
    }
    SDL_Texture* img = SDL_CreateTextureFromSurface(renderer, img_surface);
    SDL_FreeSurface(img_surface);

    Mix_Music* music = Mix_LoadMUS("music.mp3");
    if (!music) {
        printf("error loading music.mp3");
        return 1;
    }
    Mix_PlayMusic(music, -1);

    SDL_Event event;
    int running = 1;
    int setup = 1;
    int a = 0;
    unsigned long int frame_start, frame_time;

    while (running) {
        frame_start = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (setup && event.button.button == SDL_BUTTON_LEFT) {
                        int y = event.button.y / SIZE, x = event.button.x / SIZE;
                        game_state[y][x] = !game_state[y][x];
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_SPACE)
                        setup = !setup;
            }
        }

        if (!setup && a % 10 == 0)
            update();
        a %= 10;
        a++;

        SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xFF);
        SDL_RenderClear(renderer);
        draw(renderer);
        SDL_Rect img_rect = { 0, HEIGHT - 20, WIDTH, 20 };
        SDL_RenderCopy(renderer, img, NULL, &img_rect);
        SDL_RenderPresent(renderer);

        frame_time = SDL_GetTicks() - frame_start;
        if (1000 / FPS > frame_time)
            SDL_Delay(1000 / FPS - frame_time);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(img);
    Mix_FreeMusic(music);
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
    return 0;
}

int countNeighbours(int x, int y) {
    int n = 0;
    for (int i = y - 1; i < y + 2; i++)
        for (int j = x - 1; j < x + 2; j++) {
            if (i >= 0 && i < ROWS && j >= 0 && j < COLS && !(i == y && j == x))
                n += game_state[i][j];
        }
    return n;
}

void update() {
    unsigned char new_state[ROWS][COLS];

    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            int neighbours = countNeighbours(j, i);
            if (game_state[i][j] && (neighbours == 2 || neighbours == 3))
                new_state[i][j] = 1;
            else if (!game_state[i][j] && neighbours == 3)
                new_state[i][j] = 1;
            else
                new_state[i][j] = 0;
        }

    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            game_state[i][j] = new_state[i][j];
}

void draw(SDL_Renderer* renderer) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (game_state[i][j])
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            else
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_Rect rect = { j * SIZE + BORDER, i * SIZE + BORDER, CELL, CELL };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
