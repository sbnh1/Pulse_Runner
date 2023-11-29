#ifndef UTILS_H
#define UTILS_H

void  getMap(char** map, char* fileName);

SDL_Rect getZoneRect(SDL_Rect rect);

int checkCollision(SDL_Rect rect1, Block * zone);

SDL_Rect* relativPos(SDL_Rect* camera, SDL_Rect* player);

#endif