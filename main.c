#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int gridspacing = 20;

const int frametime = 1000 / 144;
int framestart;

int points[50][2] = {
    {100, 100},
    {700, 100},
    {700, 500},
    {100, 500}
};

int lines[50][2] = {
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 0}
};

void drawscreen(SDL_Renderer* render) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_RenderClear(render); 

    SDL_SetRenderDrawColor(render, 10, 10, 10, 255);
    for (int i = 0; i < 800; i += gridspacing) {
        SDL_RenderDrawLine(render, i, 0, i, 600);
    }
    for (int j = 0; j < 600; j += gridspacing) {
        SDL_RenderDrawLine(render, 0, j, 800, j);
    }

    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    for (int i = 0; i < sizeof(lines) / sizeof(lines[0]); i++) 
    {
        SDL_RenderDrawLine(render, points[lines[i][0]][0], points[lines[i][0]][1], points[lines[i][1]][0], points[lines[i][1]][1]);
    }

    SDL_RenderPresent(render);
}

int main(int argc, char *argv[]) 
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);

    if (window == NULL || renderer == NULL) 
    {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Event windowEvent;

    int follow = 0;
    int followi = 0;

    int waitingforrelease = 0;
    int size = 0;
    int newpointx = 0;
    int newpointy = 0;

    drawscreen(renderer);

    while (1)
    {
        framestart = SDL_GetTicks();
        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_QUIT == windowEvent.type)
            {break;}
            

            int x, y;
            SDL_GetMouseState(&x, &y);
            if (follow) 
            {
                points[followi][0] = x;
                points[followi][1] = y;
                
                drawscreen(renderer);
            }

            if (windowEvent.button.button == SDL_BUTTON_LEFT)
            {
                if (follow == 0)
                for (int i = 0; i < (sizeof(points) / sizeof(points[0])); i++) 
                {
                    if (abs(x - points[i][0]) <= 20 && abs(y - points[i][1]) <= 20) 
                    {
                        follow = 1;
                        followi = i;
                    }
                }
            }

            if (windowEvent.type == SDL_MOUSEBUTTONUP && windowEvent.button.button == SDL_BUTTON_LEFT && follow == 1)
            {
                points[followi][0] = round((float)x/gridspacing)*gridspacing;
                points[followi][1] = round((float)y/gridspacing)*gridspacing;
                
                drawscreen(renderer);

                follow = 0;
            }

            if (windowEvent.button.button == SDL_BUTTON_RIGHT && windowEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                newpointx = round((float)x/gridspacing)*gridspacing;
                newpointy = round((float)y/gridspacing)*gridspacing;
                waitingforrelease = 1;
            }

            if (windowEvent.button.button == SDL_BUTTON_RIGHT && windowEvent.type == SDL_MOUSEBUTTONUP && waitingforrelease == 1) 
            {
                size++;
                int newpoint2x = round((float)x/gridspacing)*gridspacing;
                int newpoint2y = round((float)y/gridspacing)*gridspacing;

                

                points[3 + size][0] = newpointx;
                points[3 + size][1] = newpointy;
                size++;

                points[3 + size][0] = newpoint2x;
                points[3 + size][1] = newpoint2y;

                lines[3 + (size/2)][0] = 3 + size - 1;
                lines[3 + (size/2)][1] = 3 + size;

                drawscreen(renderer);

                waitingforrelease = 0;
            } 
        }
        if (SDL_GetTicks() - framestart < frametime)
        {
            SDL_Delay(frametime - (SDL_GetTicks() - framestart));
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    
    return 0;
}