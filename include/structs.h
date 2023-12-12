#ifndef STRUCTS_H
#define STRUCTS_H

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

#endif 
