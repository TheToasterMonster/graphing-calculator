#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SIZE 800
#define SCALE .015f

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

uint32_t toRGB(Color* color) {
    return color->r << 24 | color->g << 16 | color->b << 8;
}

void setPixel(SDL_Surface* surface, int x, int y, uint32_t pixel) {
    uint8_t* targetPixel = (uint8_t*)surface->pixels + y * surface->pitch + x * 4;
    *(uint32_t*)targetPixel = pixel;
}

void drawAxes(SDL_Surface* pixels, uint32_t color) {
    for (int i = 0; i < SIZE; i++) {
        setPixel(pixels, i, SIZE / 2, color);
    }
    for (int j = 0; j < SIZE; j++) {
        setPixel(pixels, SIZE / 2, j, color);
    }
}

float cartesianPlane[SIZE][SIZE];
int posCounts[SIZE][SIZE];

void fillPixelGrid(SDL_Surface* pixels, uint32_t color) {
    for (int j = 0; j < SIZE; j++) {
        for (int i = 0; i < SIZE; i++) {
            if (posCounts[i][j] >= 3 && posCounts[i][j] < 6) {
                setPixel(pixels, i, j, color);
            }
        }
    }
}

void graph(SDL_Surface* pixels, float func(float), uint32_t color) {
    memset(cartesianPlane, 0, sizeof(cartesianPlane));
    // calculate function values
    for (int j = 0; j < SIZE; j++) {
        for (int i = 0; i < SIZE; i++) {
            int x = i - SIZE / 2;
            int y = -j + SIZE / 2;
            cartesianPlane[i][j] = y * SCALE - func(x * SCALE);
        }
    }

    memset(posCounts, 0, sizeof(posCounts));
    // count how many neighboring positive cells are in neighboring 3x3
    for (int j = 1; j < SIZE - 1; j++) {
        for (int i = 1; i < SIZE - 1; i++) {
            for (int dj = -1; dj < 2; dj++) {
                for (int di = -1; di < 2; di++) {
                    if (cartesianPlane[i + di][j + dj] > 0) {
                        posCounts[i][j]++;
                    }
                }
            }
        }
    }

    for (int j = 0; j < SIZE; j++) {
        for (int i = 0; i < SIZE; i++) {
            if (posCounts[i][j] >= 3 && posCounts[i][j] < 6) {
                setPixel(pixels, i, j, color);
            }
        }
    }
}

float f(float x) {
    return sin(x);
}


int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Graphing Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZE, SIZE, SDL_WINDOW_SHOWN);
    SDL_Surface* screen = SDL_GetWindowSurface(window);
    SDL_Surface* pixels = SDL_CreateRGBSurfaceWithFormat(0, SIZE, SIZE, 32, SDL_PIXELFORMAT_RGBX8888);
    
    Color bgColor = { 255, 255, 255 };
    SDL_FillRect(pixels, NULL, toRGB(&bgColor));
    Color axisColor = { 0, 0, 0 };
    drawAxes(pixels, toRGB(&axisColor));
    Color lineColor = { 255, 0, 255 };
    graph(pixels, f, toRGB(&lineColor));
    SDL_BlitSurface(pixels, NULL, screen, NULL);
    SDL_UpdateWindowSurface(window);

    while (true) {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
    }

    SDL_Quit();
    return 0;

    return 0;
}
