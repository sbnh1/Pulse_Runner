// gcc -Iinclude src/*.c -o main -lSDL2 -lSDL2_image

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "structs.h"
#include "utils.h"
#include "fonctions_SDL.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600


/*int gameLoop(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* imageSurface, SDL_Texture*, SDL_Event event, char** map) {
  return 0;
}*/

int main(void) {

    SDL_Event evenements; // Événements liés à la fenêtre
    bool terminer = false;

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

    // On prend les dimension de l'ecran pour que le personnage ne puisse pas sortir de l'écran
    int screenWidth, screenHeight;
    SDL_GetWindowSize(window, &screenWidth, &screenHeight);


    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        printf("Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        return 0;
    }

    //Initialisation de SDL_Image
    if(IMG_Init(IMG_INIT_PNG) < 0){
        SDL_Log("Erreur lors de l'initialisation de SDL_image : %s",IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    
    SDL_Surface *imageSurface = IMG_Load("sprites/fond.bmp");
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
        /*  MENU DU JEU  */
    SDL_Surface* menuSurface = IMG_Load("sprites/menu.png");  
    if (!menuSurface) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);  
    SDL_FreeSurface(menuSurface);

    if (!menuTexture) {
        SDL_Log("Erreur lors de la création de la texture : %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
//
    /* MENU DU SHOP */

    SDL_Surface* menuShopSurface = IMG_Load("sprites/menuShop.png");  
    if (!menuShopSurface) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Texture* menuShopTexture = SDL_CreateTextureFromSurface(renderer, menuShopSurface);  
    SDL_FreeSurface(menuShopSurface);

    if (!menuShopTexture) {
        SDL_Log("Erreur lors de la création de la texture : %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
//

    //variable utilisé dans le while

    // Charger l’image en fond
    SDL_Texture* fond = chargerTexture( "sprites/menu.png", renderer );
    bool enMenu = true;
    bool enShop = false;



   while (!terminer) {
        SDL_RenderClear(renderer);

        if (enMenu && !enShop) {
            SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
            SDL_RenderPresent(renderer);

            while (SDL_PollEvent(&evenements)) {
                switch (evenements.type) {
                    case SDL_QUIT:
                        terminer = true;
                        break;
                    case SDL_KEYDOWN:
                        if (evenements.key.keysym.sym == SDLK_s) {
                            enMenu = false;
                            enShop = false;
                        } else if (evenements.key.keysym.sym == SDLK_q || evenements.key.keysym.sym == SDLK_ESCAPE) {
                            terminer = true;
                        }
                        break;
                        case SDL_MOUSEBUTTONDOWN:
                            int mouseX = 0;
                            int mouseY = 0;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            if (mouseX >= 0 && mouseX <= 100 && mouseY >= 0 && mouseY <= 100) {// pour aller dans le shop
                                if(evenements.button.button ==  SDL_BUTTON_LEFT){
                                    enMenu = false;
                                    enShop = true;                                    
                                }
                            }
                            break;
                }
            }
        } else if (!enMenu && enShop){
            SDL_RenderCopy(renderer,menuShopTexture, NULL, NULL);
            SDL_RenderPresent(renderer);

            while(SDL_PollEvent(&evenements)){
                switch(evenements.type){
                    case SDL_QUIT:
                        terminer = true;
                        break;
                    case SDL_KEYDOWN:
                        if(evenements.key.keysym.sym == SDLK_p){ //pour aller dans le jeu directement
                            enShop = false;
                            enMenu = false;
                        }
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        int mouseX= 0;
                        int mouseY = 0;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        if (mouseX >= 0 && mouseX <= 100 && mouseY >= 0 && mouseY <= 100) {// pour aller dans le shop
                            if(evenements.button.button ==  SDL_BUTTON_LEFT){
                                enMenu = true;
                                enShop = false;                                    
                            }
                        }
                    break;
                }
            }
        } else if (!enMenu && !enMenu){ //la ou on va choisir les niveaux
            SDL_RenderCopy(renderer, fond, NULL, NULL);
            SDL_RenderCopy(renderer, imageTexture, NULL, NULL);

            while (SDL_PollEvent(&evenements)) {
                switch(evenements.type){
                    case SDL_QUIT:
                        terminer = true;
                        break;
                    case SDL_KEYDOWN:
                        if (evenements.key.keysym.sym == SDLK_q) {
                            terminer = true;
                        }
                        break;
                }
            }
        }

        SDL_RenderPresent(renderer);
    }
    char** map = malloc(sizeof(char*) * 11);
    
    for(int i = 0; i < 11; i++)
        map[i] = malloc(sizeof(char) * 1000);

    //getMap(map, argv[1]);


    SDL_FreeSurface(imageSurface);

    //SDL_Event event;

   // gameLoop(window, renderer, imageSurface, imageTexture, event, map);

    for (int i = 0; i < 11; i++)
        free(map[i]);

    free(map);


    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(menuTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    IMG_Quit();

    return 0;
}