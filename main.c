#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

FILE *fptr; 

int width = 800;
int height = 600;

double gridspacing = 20;
float gridspacingworth = 1;
int originx = 400;
int originy = 300;

const int frametime = 1000 / 144;
int framestart;

int followi = 0;

int newx;
int newy;

int line1;
int line2;

int x;
int y;

float points[100][2] = {
    {-10, -10},
    {10, -10},
    {10, 10},
    {-10, 10}
};

int lines[100][2] = {
    {0, 1},
    {1, 2},
    {2, 3},
    {3, 0}
};

int position = 0;

int findline(int a) {
    for (int i = 0; i < 50; i++) 
    {
        if (lines[i][0] == a) 
        {
            position = 0;
            return i;
        }
        if (lines[i][1] == a) 
        {
            position = 1;
            return i;
        }
    }
    return -1;
}

void drawscreen(SDL_Renderer* render) {
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_RenderClear(render); 

    SDL_SetRenderDrawColor(render, 10, 10, 10, 255);
    for (int i = originx; i < width; i += gridspacing) 
    {
        SDL_RenderDrawLine(render, i, 0, i, height);
    }
    for (int i = originx; i > 0; i -= gridspacing) 
    {
        SDL_RenderDrawLine(render, i, 0, i, height);
    }
    for (int i = originy; i < height; i += gridspacing) 
    {
        SDL_RenderDrawLine(render, 0, i, width, i);
    }
    for (int i = originy; i > 0; i -= gridspacing) 
    {
        SDL_RenderDrawLine(render, 0, i, width, i);
    }

    SDL_SetRenderDrawColor(render, 255, 0, 0, 255);

    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    for (int i = 0; i < 50; i++) 
    {
        SDL_RenderDrawLine(render, 
        (points[lines[i][0]][0] / gridspacingworth) * gridspacing + originx,
        (points[lines[i][0]][1] / gridspacingworth) * gridspacing + originy,
        (points[lines[i][1]][0] / gridspacingworth) * gridspacing + originx,
        (points[lines[i][1]][1] / gridspacingworth) * gridspacing + originy);
    }

    SDL_RenderPresent(render);
}

int main(int argc, char *argv[]) 
{
    fptr = fopen("keyboard-save.kbdcat", "r");
    char point[10000];
    char line[10000];
    int size = 0;
    char sizechar[1000];
    fgets(point, 10000, fptr);
    fgets(line, 10000, fptr);
    fgets(sizechar, 1000, fptr);
    fclose(fptr);

    size = atoi(sizechar);

    char* token = strtok(point, ",");

    for (int i = 0; i < 100; i++)
    {
        if (token == NULL)
        {
            token = "0";
        }
        points[i][0] = atof(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            token = "0";
        }
        points[i][1] = atof(token);
        token = strtok(NULL, ",");
    }

    token = strtok(line, ",");

    for (int i = 0; i < 100; i++)
    {
        if (token == NULL)
        {
            token = "0";
        }
        lines[i][0] = atoi(token);
        token = strtok(NULL, ",");
        if (token == NULL)
        {
            token = "0";
        }
        lines[i][1] = atoi(token);
        token = strtok(NULL, ",");
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    SDL_SetWindowResizable(window, 1);

    if (window == NULL || renderer == NULL) 
    {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Event windowEvent;

    int follow = 0;

    int moving = 0;
    int middleclickx = 0;
    int middleclicky = 0;

    int waitingforrelease = 0;
    float newpointx = 0;
    float newpointy = 0;

    int widthbefore = width;
    int heightbefore = height;

    drawscreen(renderer);

    while (1)
    {
        framestart = SDL_GetTicks();

        SDL_GetWindowSize(window, &width, &height);

        if (width != widthbefore || height != heightbefore)
        {
            widthbefore = width;
            heightbefore = height;
            drawscreen(renderer);
        }
        
        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_QUIT == windowEvent.type)
            {
                fptr = fopen("keyboard-save.kbdcat", "w");
                for (int i = 0; i < 100; i++) {
                    fprintf(fptr, "%f,%f,", points[i][0], points[i][1]);
                    
                }
                fprintf(fptr, "\n");
                for (int i = 0; i < 100; i++) {
                    fprintf(fptr, "%d,%d,", lines[i][0], lines[i][1]);
                    printf("%d,%d,", lines[i][0], lines[i][1]);
                }
                fprintf(fptr, "\n");
                fprintf(fptr, "%d", size);
                fclose(fptr);
                break;
            }

            SDL_GetMouseState(&x, &y);
            if (follow)
            {
                points[followi][0] = (float)(x - originx) / (gridspacing / gridspacingworth);
                points[followi][1] = (float)(y - originy) / (gridspacing / gridspacingworth);
                
                drawscreen(renderer);
            }

            if (moving)
            {
                originx += x - middleclickx;
                originy += y - middleclicky;
                middleclickx = x;
                middleclicky = y;
                drawscreen(renderer);
            }
            

            if (windowEvent.button.button == SDL_BUTTON_LEFT)
            {
                if (follow == 0)
                for (int i = 0; i < (sizeof(points) / sizeof(points[0])); i++) 
                {
                    if (abs(x - ((points[i][0] / gridspacingworth) * gridspacing + originx)) <= 20 && abs(y - ((points[i][1] / gridspacingworth) * gridspacing + originy)) <= 20) 
                    {
                        follow = 1;
                        followi = i;
                        break;
                    }
                }
            }

            if (windowEvent.type == SDL_MOUSEBUTTONUP && windowEvent.button.button == SDL_BUTTON_LEFT && follow == 1)
            {
                float newx = (round((float)((x - originx) / (gridspacing)))*gridspacingworth);
                float newy = (round((float)((y - originy) / (gridspacing)))*gridspacingworth);

                for (int i = 0; i < (sizeof(points) / sizeof(points[0])); i++) 
                {
                    if (i != followi && points[i][0] == newx && points[i][1] == newy) 
                    {
                        lines[findline(followi)][position] = i;
                    }
                }

                points[followi][0] = newx;
                points[followi][1] = newy;

                drawscreen(renderer);

                follow = 0;
            }

            if (windowEvent.button.button == SDL_BUTTON_RIGHT && windowEvent.type == SDL_MOUSEBUTTONDOWN && follow == 0 && waitingforrelease == 0)
            {
                newpointx = (round((float)((x - originx) / (gridspacing)))*gridspacingworth);
                newpointy = (round((float)((y - originy) / (gridspacing)))*gridspacingworth);
                waitingforrelease = 1;
            }

            if (windowEvent.button.button == SDL_BUTTON_RIGHT && windowEvent.type == SDL_MOUSEBUTTONUP && waitingforrelease == 1 && follow == 0) 
            {
                size++;
                float newpoint2x = round((float)((x - originx) / (gridspacing)))*gridspacingworth;
                float newpoint2y = round((float)((y - originy) / (gridspacing)))*gridspacingworth;

                
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

            if (SDL_MOUSEWHEEL == windowEvent.type)
            {
                if (windowEvent.wheel.y > 0)
                {
                    if (gridspacing >= 45) {
                        gridspacingworth /= 5;
                        gridspacing /= 5;
                    }

                    gridspacing += 5;

                    originx = x - ((x - originx) * (gridspacing + 5) / gridspacing);
                    originy = y - ((y - originy) * (gridspacing + 5) / gridspacing);
                }
                else if (windowEvent.wheel.y < 0)
                {
                    if (gridspacing - 5 <= 1)
                    {
                        gridspacingworth *= 5;
                        gridspacing *= 5; 
                    }
                    gridspacing -= 5;
                }

                drawscreen(renderer);
            }

            if (windowEvent.button.button == SDL_BUTTON_MIDDLE && windowEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                moving = 1;
                middleclickx = x;
                middleclicky = y;
            }

            if (windowEvent.button.button == SDL_BUTTON_MIDDLE && windowEvent.type == SDL_MOUSEBUTTONUP)
            {
                moving = 0;
            }

            if (windowEvent.key.keysym.sym == SDLK_d && windowEvent.type == SDL_KEYDOWN && follow == 1)
            {
                if (findline(followi) == -1)
                {
                    int remove = findline(followi);
                    lines[remove][0] = -1;
                    lines[remove][1] = -1;
                }
                else
                {
                    int remove = findline(followi);
                    lines[remove][0] = -1;
                    lines[remove][1] = -1;
                }
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