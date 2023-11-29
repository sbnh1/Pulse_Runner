#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "fonctions_SDL.h"

// Fonction pour charger une image et créer une texture SDL_Texture*
SDL_Texture* chargerTexture(const char* nomfichier, SDL_Renderer* renderer) {
    // Charger l'image en utilisant SDL_image
    SDL_Surface* surface = IMG_Load(nomfichier);
    if (!surface) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        return NULL;
    }

    // Créer une texture à partir de la surface de l'image
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);  // Libérer la surface car elle n'est plus nécessaire

    if (!texture) {
        SDL_Log("Erreur lors de la création de la texture : %s", SDL_GetError());
        return NULL;
    }

    return texture;
}

SDL_Texture* charger_image_transparente(const char* nomfichier, SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b) {
    // Charger l'image en utilisant SDL_image
    SDL_Surface* surface = IMG_Load(nomfichier);
    if (!surface) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        return NULL;
    }

    // Définir la couleur transparente (color key)
    Uint32 colorKey = SDL_MapRGB(surface->format, r, g, b);
    if (SDL_SetColorKey(surface, SDL_TRUE, colorKey) != 0) {
        SDL_Log("Erreur lors de la définition de la couleur transparente : %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    // Créer une texture à partir de la surface de l'image
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);  // Libérer la surface car elle n'est plus nécessaire

    if (!texture) {
        SDL_Log("Erreur lors de la création de la texture : %s", SDL_GetError());
        return NULL;
    }

    return texture;
}


SDL_Texture* charger_texte(const char* message, SDL_Renderer* renderer, TTF_Font* font, SDL_Color color) {
    
    if (TTF_Init() < 0) {
        SDL_Log("Erreur lors de l'initialisation de SDL_ttf : %s", TTF_GetError());
        TTF_Quit();
        return NULL;
    }

    // Création du texte
    SDL_Surface* texte = TTF_RenderText_Solid(font, message, color);
    if (!texte) {
        SDL_Log("Erreur lors du rendu du texte : %s", TTF_GetError());
        TTF_Quit();
        return NULL;
    }

    // Création de l'image du texte
    SDL_Texture* imagetexte = SDL_CreateTextureFromSurface(renderer, texte);
    SDL_FreeSurface(texte);  

    if (!imagetexte) {
        SDL_Log("Erreur lors de la création de la texture du texte : %s", SDL_GetError());
        TTF_Quit();  
        return NULL;
    }

    return imagetexte;
}
