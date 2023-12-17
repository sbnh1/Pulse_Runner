#include <SDL2/SDL.h>
#include <stdbool.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "fonctions_SDL.h"
#include "structs.h"

SDL_Texture* chargerTexture(const char* nomfichier, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(nomfichier);
    if (!surface) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);  

    if (!texture) {
        SDL_Log("Erreur lors de la création de la texture : %s", SDL_GetError());
        return NULL;
    }

    return texture;
}

SDL_Texture* charger_image_transparente(const char* nomfichier, SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Surface* surface = IMG_Load(nomfichier);
    if (!surface) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        return NULL;
    }
    Uint32 colorKey = SDL_MapRGB(surface->format, r, g, b);
    if (SDL_SetColorKey(surface, SDL_TRUE, colorKey) != 0) {
        SDL_Log("Erreur lors de la définition de la couleur transparente : %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); 

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
    SDL_Surface* texte = TTF_RenderText_Solid(font, message, color);
    if (!texte) {
        SDL_Log("Erreur lors du rendu du texte : %s", TTF_GetError());
        TTF_Quit();
        return NULL;
    }
    SDL_Texture* imagetexte = SDL_CreateTextureFromSurface(renderer, texte);
    SDL_FreeSurface(texte);  
    if (!imagetexte) {
        SDL_Log("Erreur lors de la création de la texture du texte : %s", SDL_GetError());
        TTF_Quit();  
        return NULL;
    }

    return imagetexte;
}

//fonction qui créer un bouton
Button createButton(SDL_Renderer* renderer, const char* imagePath, int x, int y, int w, int h) {
    Button button;
    button.rect.x = x;
    button.rect.y = y;
    button.rect.w = w;
    button.rect.h = h;
    button.texture = SDL_CreateTextureFromSurface(renderer, IMG_Load(imagePath));
    return button;
}    

//fonction qui affiche le bouton quand on survol un bouton
SDL_Texture* loadHoverTexture(SDL_Renderer* renderer, const char* imagePath) {
    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface) {
        printf("Erreur IMG_Load: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Erreur SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        return NULL;
    }
    return texture;
}

//fonction qui dit si la souris survol un bouton
bool isMouseOnButton(int mouseX, int mouseY, SDL_Rect buttonRect) {
    return (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
            mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h);
}

void freeButtons(Button* buttons, int count) {
    for (int i = 0; i < count; i++) {
        SDL_DestroyTexture(buttons[i].texture);
        if (buttons[i].hoverTexture != NULL) {
            SDL_DestroyTexture(buttons[i].hoverTexture);
        }
    }
}

void afficherFPS(SDL_Renderer* ecran, TTF_Font* font, int* fpsCounter, int* fps) {
    Uint32 currentTime = SDL_GetTicks();
    Uint32 deltaTime = currentTime;
    const int FPS = 60;
    unsigned int timetime = 1000;
    const int DELAY_TIME = 1000 / FPS;

    (*fpsCounter)++;
    Uint32 frameStart = SDL_GetTicks();

    if (deltaTime >= timetime) {
        *fps = *fpsCounter;
        *fpsCounter = 0;
        timetime += 1000;
    }

    char fpsText[16];
    sprintf(fpsText, "FPS: %d", *fps);

    SDL_Color textColor = {255, 255, 255, 255}; // Couleur du texte (blanc)
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, fpsText, textColor);
    if (!textSurface) {
        SDL_Log("Erreur lors de la création de la surface de texte : %s", TTF_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(ecran, textSurface);
    if (!textTexture) {
        SDL_Log("Erreur lors de la création de la texture de texte : %s", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Libérez la surface de texte, elle n'est plus nécessaire
    SDL_FreeSurface(textSurface);

    SDL_Rect textRect;
    textRect.x = 910; 
    textRect.y = 10; 
    textRect.w = textSurface->w; 
    textRect.h = textSurface->h; 

    // Affichez la texture de texte à l'écran
    SDL_RenderCopy(ecran, textTexture, NULL, &textRect);

    // Détruisez la texture de texte lorsque vous n'en avez plus besoin
    SDL_DestroyTexture(textTexture);

    Uint32 frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < (Uint32)DELAY_TIME) {
        SDL_Delay((Uint32)(DELAY_TIME - frameTime));
    }
}
