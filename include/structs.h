#ifndef STRUCTS_H
#define STRUCTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


typedef struct Player { 
    SDL_Rect rect;
    double speedY;
    int speedX;
    int state;   
    SDL_Texture* sprite_image;
} Player; 

typedef struct Block{
    char type;
    SDL_Rect rect;
    SDL_Rect hitBox;
    SDL_Texture* sprite_image;
} Block;

typedef struct {
    SDL_Texture* texture;
    SDL_Texture* hoverTexture;  
    SDL_Rect rect;
} Button;

typedef struct Skin{
    char* path;
    int isUnblocked;
} Skin;

typedef struct List{
    Skin skin;
    struct List* next;
} List;

typedef struct {
    Uint8 r; 
    Uint8 g; 
    Uint8 b; 
    Uint8 a; 
} MyColor;


#endif 
