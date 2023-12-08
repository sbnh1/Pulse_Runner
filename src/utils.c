#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "structs.h"
#include "utils.h"

int checkCollision(SDL_Rect rect1, Block * zone) {
    for (int i = 0; i < 4; i++){
        if(SDL_HasIntersection(&rect1, &zone[i].rect))
        return 1;
    }
    return 0;
}

void getMap(char** map, char* fileName){
    
    FILE* file = fopen(fileName, "r");

    char line[1000];

    for(int i = 0; fgets(line, sizeof(line), file) != NULL; i++)
        if(strlen(line) > 2)
            for(int j = 0; j < strlen(line); j++){
                if(line[j] == '0' || line[j] == '1')
                    map[i][j] = line[j];
            }

    
    fclose(file);
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

