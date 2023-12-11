#ifndef FONCTIONS_SDL_H
#define FONCTIONS_SDL_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "structs.h"


SDL_Texture* chargerTexture(const char* nomfichier, SDL_Renderer* renderer);

SDL_Texture* charger_image_transparente(const char* nomfichier, SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b);

SDL_Texture* charger_texte(const char* message, SDL_Renderer* renderer, TTF_Font *font, SDL_Color color);

Button createButton(SDL_Renderer* renderer, const char* imagePath, int x, int y, int w, int h);

#endif