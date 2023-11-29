// gcc -Iinclude src/*.c -o main -lSDL2 -lSDL2_image

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "structs.h"
#include "utils.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600

int gameLoop(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* imageSurface, SDL_Texture*, SDL_Event event, char** map) {
    return 0;
}

int main(int argc, char *argv[]) {

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Erreur SDL_Init: %s\n",SDL_GetError());
        return 0;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("Erreur IMG_Init: %s\n", IMG_GetError());
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow("Ceometry dash", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if(!window){
        printf("Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        return 0;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        printf("Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        return 0;
    }
    
    SDL_Surface *imageSurface = IMG_Load("sprite_1.png");
    if (!imageSurface) {
        printf("Erreur IMG_Load: %s\n", IMG_GetError());
        return 1;
    }

    SDL_Texture *imageTexture = NULL;
    imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    if (!imageTexture) {
        printf("Erreur SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        return 1;
    }

    char** map = malloc(sizeof(char*) * 11);
    
    for(int i = 0; i < 11; i++)
        map[i] = malloc(sizeof(char) * 1000);

    getMap(map, argv[1]);


    SDL_FreeSurface(imageSurface);

    SDL_Event event;

    gameLoop(window, renderer, imageSurface, imageTexture, event, map);

    for (int i = 0; i < 11; i++)
        free(map[i]);

    free(map);


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}