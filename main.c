#include <SDL.h>
#include <stdio.h>

#include "matrix.h"

#define WIDTH   1280            // in pixels
#define HEIGHT  720             // in pixels
#define TITLE   "Game of life"

#define FRAMES_PER_SECOND   30
#define DELAY   (1000 / FRAMES_PER_SECOND)

#define STEPS_PER_FRAME 1

#define CELL_SIZE    5                      // cell size in pixels
#define CELL_COUNTX  (WIDTH / CELL_SIZE)    // width of grid of cells
#define CELL_COUNTY  (HEIGHT / CELL_SIZE)   // height of grid of cells

SDL_Window* WindowCreate(int width, int height, char* title)
{
    // Initialize SDL with video resources
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("[error] SDL video subsystem could not initialize! SDL_Error: %s\n", SDL_GetError());

    // Create SDL window
    SDL_Window* context = SDL_CreateWindow(title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height, SDL_WINDOW_SHOWN);

    if (context == NULL)
        printf("[error] Window could not be created! SDL_Error: %s\n", SDL_GetError());

    return context;
}

SDL_Surface* SurfaceCreate(SDL_Window* context)
{
    // Get the surface of the current window
    SDL_Surface* surface = SDL_GetWindowSurface(context);

    if (surface == NULL)
        printf("[error] Surface could not be created! SDL_Error: %s\n", SDL_GetError());

    return surface;
}

void WindowUpdate(SDL_Window* context)
{
    // Update the pixels surface
    if (SDL_UpdateWindowSurface(context) == -1)
        printf("[error] Surface could not be updated! SDL_Error: %s\n", SDL_GetError());
}

void Paint(SDL_Surface* surface, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    // Paint the (x, y) cell
    SDL_Rect cell = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    SDL_FillRect(surface, &cell, SDL_MapRGB(surface->format, r, g, b));
}

void Draw(SDL_Surface* surface, int** grid, int rows, int columns)
{
    // Paint each cell of the grid
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            Uint8 color = grid[i][j] * 0xFF;

            Paint(surface, i, j, color, color, color);
        }
    }
}

int InputEvent()
{
    SDL_Event event;
    int quit = 0;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: quit = 1; break; // if close the window
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: quit = 1; break; // if press escape key
            default: break;
            } break;
        default: break;
        }
    }

    return quit;
}

int count_neighbors(int** grid, int x, int y)
{
    int result = 0;

    int x_not_min = x > 0;
    int x_not_max = x < CELL_COUNTX - 1;

    int y_not_min = y > 0;
    int y_not_max = y < CELL_COUNTX - 1;

    if (x_not_min) {
        if (y_not_min)
            result += grid[x - 1][y - 1];

        result += grid[x - 1][y];

        if (y_not_max)
            result += grid[x - 1][y + 1];
    }

    if (x_not_max) {
        if (y_not_min)
            result += grid[x + 1][y - 1];

        result += grid[x + 1][y];

        if (y_not_max)
            result += grid[x + 1][y + 1];
    }

    if (y_not_min)
        result += grid[x][y - 1];
    
    if (y_not_max)
        result += grid[x][y + 1];

    return result;
}

void life_iterate(int** grid, int** aux)
{
    copy_matrix(grid, aux, CELL_COUNTX, CELL_COUNTY);

    for (int i = 0; i < CELL_COUNTX; i++) {
        for (int j = 0; j < CELL_COUNTY; j++) {
            int neighbors = count_neighbors(aux, i, j);

            if (aux[i][j] == 0 && neighbors == 3) {
                grid[i][j] = 1;
            }
            else if (aux[i][j] == 1 && (neighbors == 2 || neighbors == 3)) {
                grid[i][j] = 1;
            }
            else {
                grid[i][j] = 0;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL)); // set random seed

    SDL_Window* context = WindowCreate(WIDTH, HEIGHT, TITLE);
    SDL_Surface* surface = SurfaceCreate(context);

    int** grid = create_matrix(CELL_COUNTX, CELL_COUNTY);
    int** aux = create_matrix(CELL_COUNTX, CELL_COUNTY);

    for (int i = 0; i < CELL_COUNTX; i++) {
        for (int j = 0; j < CELL_COUNTY; j++) {
            grid[i][j] = rand() % 2;
        }
    }

    int quit = 0;

    int steps_count = 0;
    char title[32];

    while (!quit) {
        quit = InputEvent();

        for (int i = 0; i < STEPS_PER_FRAME; i++) {
            life_iterate(grid, aux);

            steps_count++;
        }

        Draw(surface, grid, CELL_COUNTX, CELL_COUNTY);

        WindowUpdate(context);

        sprintf_s(title, 32, "%s | %d steps", TITLE, steps_count);
        SDL_SetWindowTitle(context, title);

        SDL_Delay(DELAY);
    }

    aux = destroy_matrix(aux, CELL_COUNTX, CELL_COUNTY);
    grid = destroy_matrix(grid, CELL_COUNTX, CELL_COUNTY);

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(context);
    SDL_Quit();

    return 0;
}