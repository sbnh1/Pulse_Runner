#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "structs.h"
#include "utils.h"

#include <SDL2/SDL.h>

void updateDataFile(char* map) {
    FILE *file;
    char buffer[100];

    file = fopen("data", "r+");

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char currentMap[50];
        int value;

        if (sscanf(buffer, "%s %d", currentMap, &value) == 2) {
            if (strcmp(currentMap, map) == 0) {
                fseek(file, -strlen(buffer), SEEK_CUR);
                fprintf(file, "%s 1\n", currentMap);
            }
        }
    }

    fclose(file);
}

int* lireData(const char* file) {
    FILE* fichier = fopen(file, "r");
    
    int* tab = (int*)malloc(3 * sizeof(int));

    for (int i = 0; i < 3; i++) {
        char nomMap[10];
        int val;

        if (fscanf(fichier, "%s %d", nomMap, &val) == 2)
            tab[i] = val;
    }

    fclose(fichier);

    return tab;
}

void freeMap(Block** blockMap){
    for (int i = 0; i < 11; i++){
        for (int j = 0; j < 1000; j++){
            if(blockMap[i][j].sprite_image != NULL)
                SDL_DestroyTexture(blockMap[i][j].sprite_image);
            blockMap[i][j].sprite_image = NULL;
        }
        free(blockMap[i]);        
    }
    free(blockMap);
}

int checkCollision(SDL_Rect rect1, Block * zone) {
    for (int i = 0; i < 4; i++){
        if(SDL_HasIntersection(&rect1, &zone[i].hitBox)) 
            return zone[i].type;
    }
    return -1;
}

Block** getMap(char* fileName, SDL_Renderer *renderer){
    
    FILE* file = fopen(fileName, "r");

    char line[1000];

    Block** blockMap = malloc(sizeof(Block*) * 11);
    for (int i = 0; i < 11; i++){
        blockMap[i] = malloc(sizeof(Block) * 1000);
        for(int j = 0; j < 1000; j++){
            blockMap[i][j].type = '0';
            blockMap[i][j].sprite_image = NULL;
        }
    }

    SDL_Rect blockRect;
    SDL_Rect hitBox;

    for(int i = 0; fgets(line, sizeof(line), file) != NULL; i++){
        line[strcspn(line, "\n")] = '\0';
        if(strlen(line) > 2){
            for(int j = 0; j < strlen(line); j++){
                switch (line[j]) {      
                    case '1':
                        blockRect.x = j * 50;
                        blockRect.y = i * 50;
                        blockRect.w = 50;
                        blockRect.h = 50;
                        blockMap[i][j].rect = blockRect;
                        blockMap[i][j].hitBox = blockRect;
                        blockMap[i][j].sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load("sprites/block.png"));
                        if (blockMap[i][j].sprite_image == NULL){
                        }
                        blockMap[i][j].type = '1';
                        break;
                    case '2':
                        blockRect.x = j * 50;
                        blockRect.y = i * 50;
                        blockRect.w = 50;
                        blockRect.h = 50;
                        hitBox.x = blockRect.x + 20; 
                        hitBox.y = blockRect.y + 10;
                        hitBox.w = 10;
                        hitBox.h = 18;
                        blockMap[i][j].rect = blockRect;
                        blockMap[i][j].hitBox = hitBox;
                        blockMap[i][j].sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load("sprites/spike.png"));
                        if (blockMap[i][j].sprite_image == NULL){
                        }
                        blockMap[i][j].type = '2';
                        break;
                    case '3':
                        blockRect.x = j * 50;
                        blockRect.y = i * 50;
                        blockRect.w = 50;
                        blockRect.h = 50;
                        blockMap[i][j].rect = blockRect;
                        blockMap[i][j].hitBox = blockRect;
                        blockMap[i][j].sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load("sprites/end_block.png"));
                        if (blockMap[i][j].sprite_image == NULL){
                        }
                        blockMap[i][j].type = '3';
                        break;
                }
            }
        }    
    }    
    fclose(file);
    return blockMap;
}

SDL_Rect getZoneRect(SDL_Rect rect){
    SDL_Rect resRect = {
        rect.x-(rect.x%50),
        rect.y-(rect.y%50),
        100,
        100
    };
    return resRect;
}


SDL_Rect* relativPos(SDL_Rect* camera, SDL_Rect* player) {
    SDL_Rect* res = malloc(sizeof(SDL_Rect));
    res->x = player->x - camera->x;
    res->y = player->y - camera->y;
    res->w = player->w;
    res->h = player->h;

    return res;
}

void renderBackground(SDL_Renderer *renderer, SDL_Texture *backgroundTexture, int scrollOffset, int SCREEN_WIDTH, int SCREEN_HEIGHT, int TILE_SIZE) {
    for (int y = 0; y < SCREEN_HEIGHT; y += TILE_SIZE) {
        for (int x = -SCREEN_WIDTH; x < SCREEN_WIDTH*2; x += TILE_SIZE) {
            SDL_Rect destRect = {x - scrollOffset, y, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
        }
    }
}