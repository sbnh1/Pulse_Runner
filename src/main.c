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

#define TEST(i) printf("%d\n", i);



int gameLoop(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* imageSurface, SDL_Texture* imageTexture, SDL_Event event, char** map) {

    int targetFPS = 100;
    int targetFrameTime = 1000 / targetFPS;

    int elapsedTime;
    Uint32 lastTick = SDL_GetTicks();
    Uint32 currentTick = SDL_GetTicks();

    Block** blockMap = malloc(sizeof(Block*) * 11);

    for (int i = 0; i < 11; i++) {
        blockMap[i] = malloc(sizeof(Block) * 1000);
    }

    SDL_Rect blockRect;

    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 1000; j++) {
            blockMap[i][j].type = '0';
            switch (map[i][j]) {
                case '1':
                    blockRect.x = j * 50;
                    blockRect.y = i * 50;
                    blockRect.w = 50;
                    blockRect.h = 50;
                    blockMap[i][j].rect = blockRect;
                    blockMap[i][j].sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load("block.png"));
                    blockMap[i][j].type = '1';
                    break;
            }
        }
    }
    
    while (1) {
        TEST(1)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                goto end;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 11; i++) {
            for (int j = 0; j <  1000; j++) {
                switch (blockMap[i][j].type) {
                    case '1':
                        SDL_RenderCopy(renderer, blockMap[i][j].sprite_image, NULL, &blockMap[i][j].rect);
                        break;
                }
            }
        }

        SDL_RenderPresent(renderer);

        currentTick = SDL_GetTicks();
        elapsedTime = SDL_GetTicks() - lastTick;

        if (elapsedTime < targetFrameTime) {
            SDL_Delay(targetFrameTime - elapsedTime);
        }
        lastTick = currentTick;
    }

end:

    for (int i = 0; i < 11; i++)
        free(blockMap[i]);

    free(blockMap);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int main(int argc, char** argv) {

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
    
    SDL_Window *window = SDL_CreateWindow("Pulse runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
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
    //
    //utilité du code ? de ici jusqu'au prochain commentaire
    //
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
    /*  CHOIX DE LA MAP*/
    SDL_Surface* choixMapSurface = IMG_Load("sprites/choixMap.png");  
    if (!choixMapSurface) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Texture* choixMapTexture = SDL_CreateTextureFromSurface(renderer, choixMapSurface);  
    SDL_FreeSurface(choixMapSurface);

    if (!choixMapTexture) {
        SDL_Log("Erreur lors de la création de la texture : %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    //variable utilisé dans le while

    // Charger l’image en fond
    SDL_Texture* fond = chargerTexture( "sprites/menu.png", renderer );


    bool enMenu = true;
    bool enShop = false;
    bool enChoix = false;

    //création des boutons
    Button buttonMenuToShop = createButton(renderer, "sprites/buttonShop.png", 350, 250, 100, 100);
    Button buttonShopToMenu = createButton(renderer, "sprites/button1.png", 20, 20, 150, 60);
    Button buttonMenuToChoice = createButton(renderer, "sprites/buttonPlay.png", 550, 250, 100, 100);
    Button buttonChoiceToMenu = createButton(renderer, "sprites/button1.png", 20, 20, 150, 60);
    Button buttonMapToChoice = createButton(renderer, "sprites/button1.png", 830, 20, 150, 60);

    //création des boutons pour le shop
    Button cube1 = createButton(renderer, "sprites/cube1.png", 300, 200, 80, 80);
    Button cube2 = createButton(renderer, "sprites/cube2.png", 450, 200, 80, 80);
    Button cube3 = createButton(renderer, "sprites/cube3.png", 600, 200, 80, 80);
    Button cube4 = createButton(renderer, "sprites/cube4.png", 300, 350, 80, 80);
    Button cube5 = createButton(renderer, "sprites/cube5.png", 450, 350, 80, 80);
    Button cube6 = createButton(renderer, "sprites/cube6.png", 600, 350, 80, 80);

    //création des boutons pour les map
    Button map1 = createButton(renderer, "sprites/gentleGlide.png", 100, 200, 200, 200);
    Button map2 = createButton(renderer, "sprites/neverGiveUp.png", 400, 200, 200, 200);
    Button map3 = createButton(renderer, "sprites/eternalAscension.png", 700, 200, 200, 200);

    char** map = malloc(sizeof(char*) * 11);
    
    for(int i = 0; i < 11; i++)
        map[i] = malloc(sizeof(char) * 1000);

   while (!terminer) {
        SDL_RenderClear(renderer);

        if (enMenu && !enShop && !enChoix) { //menu principal
            SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
            SDL_RenderCopy(renderer, buttonMenuToShop.texture, NULL, &buttonMenuToShop.rect);
            SDL_RenderCopy(renderer, buttonMenuToChoice.texture, NULL, &buttonMenuToChoice.rect);


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
                            enChoix = false;
                        } else if (evenements.key.keysym.sym == SDLK_q || evenements.key.keysym.sym == SDLK_ESCAPE) {
                            terminer = true;
                        }
                        break;
                        case SDL_MOUSEBUTTONDOWN:
                            int mouseX = 0;
                            int mouseY = 0;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            //bouton pour aller au shop
                            if (mouseX >= buttonMenuToShop.rect.x && mouseX <= buttonMenuToShop.rect.x + buttonMenuToShop.rect.w &&
                                mouseY >= buttonMenuToShop.rect.y && mouseY <= buttonMenuToShop.rect.y + buttonMenuToShop.rect.h) {
                                enShop = true;
                                enMenu = false;
                                enChoix = false;
                            }
                            //bouton pour aller au choix du lvl
                            if (mouseX >= buttonMenuToChoice.rect.x && mouseX <= buttonMenuToChoice.rect.x + buttonMenuToChoice.rect.w &&
                                mouseY >= buttonMenuToChoice.rect.y && mouseY <= buttonMenuToChoice.rect.y + buttonMenuToChoice.rect.h) {
                                enShop = false;
                                enMenu = false;
                                enChoix = true;
                            }
                            break;
                }
            }
        } else if (!enMenu && enShop && !enChoix){ //dans le shop
            SDL_RenderCopy(renderer,menuShopTexture, NULL, NULL);
            SDL_RenderCopy(renderer, buttonShopToMenu.texture, NULL, &buttonShopToMenu.rect);
            SDL_RenderCopy(renderer, cube1.texture, NULL, &cube1.rect);
            SDL_RenderCopy(renderer, cube2.texture, NULL, &cube2.rect);
            SDL_RenderCopy(renderer, cube3.texture, NULL, &cube3.rect);
            SDL_RenderCopy(renderer, cube4.texture, NULL, &cube4.rect);
            SDL_RenderCopy(renderer, cube5.texture, NULL, &cube5.rect);
            SDL_RenderCopy(renderer, cube6.texture, NULL, &cube6.rect);


            SDL_RenderPresent(renderer);

            while(SDL_PollEvent(&evenements)){
                switch(evenements.type){
                    case SDL_QUIT:
                        terminer = true;
                        break;
                    case SDL_KEYDOWN:
                        if(evenements.key.keysym.sym == SDLK_q){
                            terminer = true;
                        }
                        if(evenements.key.keysym.sym == SDLK_p){ //pour aller au choix de la map directement a enlever a la fin
                            enShop = false;
                            enMenu = false;
                            enChoix = false;
                        }
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        int mouseX= 0;
                        int mouseY = 0;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        //bouton retour en arriere
                        if (mouseX >= buttonShopToMenu.rect.x && mouseX <= buttonShopToMenu.rect.x + buttonShopToMenu.rect.w &&
                                mouseY >= buttonShopToMenu.rect.y && mouseY <= buttonShopToMenu.rect.y + buttonShopToMenu.rect.h) {
                                enShop = false;
                                enMenu = true;
                                enChoix = false;
                            }
                        //bouton en rapport au skin
                    break;
                    //rajouter les directives a prendre pour chaque personnage
                }
            }
        } else if (!enMenu && !enShop && enChoix){ //choix des niveaux
            SDL_RenderCopy(renderer, choixMapTexture, NULL, NULL);
            SDL_RenderCopy(renderer, buttonChoiceToMenu.texture, NULL, &buttonChoiceToMenu.rect);
            SDL_RenderCopy(renderer, map1.texture, NULL, &map1.rect);
            SDL_RenderCopy(renderer, map2.texture, NULL, &map2.rect);
            SDL_RenderCopy(renderer, map3.texture, NULL, &map3.rect);
            SDL_RenderPresent(renderer);


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
                    case SDL_MOUSEBUTTONDOWN:
                            int mouseX = 0;
                            int mouseY = 0;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            //bouton retour en arriere
                            if (mouseX >= buttonChoiceToMenu.rect.x && mouseX <= buttonChoiceToMenu.rect.x + buttonChoiceToMenu.rect.w &&
                                mouseY >= buttonChoiceToMenu.rect.y && mouseY <= buttonChoiceToMenu.rect.y + buttonChoiceToMenu.rect.h) {
                                enShop = false;
                                enMenu = true;
                                enChoix = false;
                            }
                            //bouton des choix de lvl
                            if (mouseX >= map1.rect.x && mouseX <= map1.rect.x + map1.rect.w &&
                                mouseY >= map1.rect.y && mouseY <= map1.rect.y + map1.rect.h) {
                                getMap(map, "map");
                                enShop = false;
                                enMenu = false;
                                enChoix = false;
                            }
                    break;

                }
            }
        } else if (!enMenu && !enShop && !enChoix){ //lancement de la map choisi
            SDL_RenderCopy(renderer, choixMapTexture, NULL, NULL);
            SDL_RenderCopy(renderer, buttonMapToChoice.texture, NULL, &buttonMapToChoice.rect);
            SDL_RenderPresent(renderer);
            
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
            SDL_Event event;
            gameLoop(window, renderer, imageSurface, imageTexture, event, map);
                for (int i = 0; i < 11; i++)
                    free(map[i]);

                free(map);
        }

        SDL_RenderPresent(renderer);
    }

//     char** map = malloc(sizeof(char*) * 11);
    
//     for(int i = 0; i < 11; i++)
//         map[i] = malloc(sizeof(char) * 1000);

//     getMap(map, argv[1]);


//     SDL_FreeSurface(imageSurface);

//     SDL_Event event;

//    gameLoop(window, renderer, imageSurface, imageTexture, event, map);

//     for (int i = 0; i < 11; i++)
//         free(map[i]);

//     free(map);


    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(menuTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    IMG_Quit();
    printf("1\n");
    return 0;
}