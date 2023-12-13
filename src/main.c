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

#define TILE_SIZE 500
#define SCROLL_SPEED 1

#define SETZONECOLLISION(zC, map, z) \
    zC[0] = map[(z.y / 50)][(z.x / 50)]; \
    zC[1] = map[(z.y / 50)][(z.x / 50) + 1]; \
    zC[2] = map[(z.y / 50) + 1][(z.x / 50)]; \
    zC[3] = map[(z.y / 50) + 1][(z.x / 50) + 1];

#define GRAVITY 0.3
#define JUMP_SPEED -9

bool enMenu = true;
bool enShop = false;
bool enChoix = false;

int gameLoop(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* imageSurface, SDL_Texture* imageTexture, SDL_Event event, char** map) {
    printf("je rentre dans gameloop\n");
    SDL_Surface *backgroundSurface = IMG_Load("sprites/background.png");
    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    SDL_SetTextureColorMod(backgroundTexture, 128, 0, 128);

    SDL_Rect basePlayerRect = {200, 150, 50, 50};
    int sortieGameLoop = 0; 

    Player player = {
        .rect = basePlayerRect, // player rect 
        .speedX = 2,                 // player speed
        .speedY = 0.0,                     //player speed y
        .state = 0,                 // state
        .sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load("sprites/cube5.png")) // player texture
    };

    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    int targetFrameTime = 15;

    int elapsedTime;
    Uint32 lastTick = SDL_GetTicks();
    Uint32 currentTick = SDL_GetTicks();

    Block** blockMap = malloc(sizeof(Block*) * 11);

    for (int i = 0; i < 11; i++) {
        blockMap[i] = malloc(sizeof(Block) * 1000);
    }

    SDL_Rect blockRect;
    SDL_Rect hitBox;

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
                    blockMap[i][j].hitBox = blockRect;
                    blockMap[i][j].sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load("sprites/block.png"));
                    blockMap[i][j].type = '1';
                    break;
                case '2':
                    blockRect.x = j * 50;
                    blockRect.y = i * 50;
                    blockRect.w = 50;
                    blockRect.h = 50;
                    hitBox.x = blockRect.x + 20; 
                    hitBox.y = blockRect.y + 10;
                    hitBox.w = 12;
                    hitBox.h = 18;
                    blockMap[i][j].rect = blockRect;
                    blockMap[i][j].hitBox = hitBox;
                    blockMap[i][j].sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load("sprites/spike.png"));
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
                    blockMap[i][j].type = '3';
                    break;
            }
        }
    }

    int gridColor = 20;

    Block* zoneCollision = malloc(sizeof(Block) * 4);

    SDL_Rect zone = getZoneRect(player.rect);

    int lastPlayerXPos = 0;

    int cpt = 0;

    int scrollOffset = 0;

    while (!sortieGameLoop) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                sortieGameLoop = 1;
            } else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                switch (key) {
                    case SDLK_SPACE:
                        if (player.state != 1) {
                            player.state = 1;
                            player.speedY = JUMP_SPEED;
                        }
                        break;
                }
            }
        }

        player.speedY += GRAVITY;

        int collision;
        int reachedEndBlock = 0;

        zone = getZoneRect(player.rect);
        SETZONECOLLISION(zoneCollision, blockMap, zone)
        player.rect.x += player.speedX;
        collision = checkCollision(player.rect, zoneCollision);
        switch(collision){
            case '1': player.rect.x -= player.speedX; break;
            case '2': player.rect = basePlayerRect; break;
            case '3': reachedEndBlock = 1; break;
        } 

        zone = getZoneRect(player.rect);
        SETZONECOLLISION(zoneCollision, blockMap, zone)
        player.rect.y += (int)player.speedY;
        collision = checkCollision(player.rect, zoneCollision);
        switch(collision){
            case '1': 
                player.state = 0;
                player.rect.y -= (int)player.speedY; 
                int remainder = player.rect.y % 50;
                if (remainder != 0) {
                    player.rect.y -= remainder;
                }
                player.speedY = 0.0;
                break;
            case '2': 
                player.rect = basePlayerRect; 
                player.speedY = 0.0;
                break;
            case '3': 
                reachedEndBlock = 1; 
                break;
        } 

        // if(lastPlayerXPos == player.rect.x)
        //     player.rect = basePlayerRect;

        camera.x = player.rect.x - 300;
        scrollOffset += SCROLL_SPEED;
        if (scrollOffset >= TILE_SIZE)
            scrollOffset = 0;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderBackground(renderer, backgroundTexture, scrollOffset, SCREEN_WIDTH, SCREEN_HEIGHT, TILE_SIZE);

        for (int i = 0; i < 11; i++) {
            for (int j = camera.x / 50; j < (camera.x + camera.w + 50) / 50; j++) {
                SDL_Rect* relativBlockPos;
                switch (blockMap[i][j].type) {
                    case '1':
                        relativBlockPos = relativPos(&camera, &blockMap[i][j].rect);
                        SDL_RenderCopy(renderer, blockMap[i][j].sprite_image, NULL, relativBlockPos);
                        free(relativBlockPos);
                        break;
                    case '2':
                        relativBlockPos = relativPos(&camera, &blockMap[i][j].rect);
                        SDL_RenderCopy(renderer, blockMap[i][j].sprite_image, NULL, relativBlockPos);
                        free(relativBlockPos);
                        break;
                    case '3':
                        relativBlockPos = relativPos(&camera, &blockMap[i][j].rect);
                        SDL_RenderCopy(renderer, blockMap[i][j].sprite_image, NULL, relativBlockPos);
                        free(relativBlockPos);
                        break;
                }
            }
        }

        SDL_Rect* relativPlayerPos = relativPos(&camera, &player.rect);
        SDL_RenderCopy(renderer, player.sprite_image, NULL, relativPlayerPos);
        free(relativPlayerPos);
        SDL_RenderPresent(renderer);

        lastPlayerXPos = player.rect.x;

        currentTick = SDL_GetTicks();
        elapsedTime = SDL_GetTicks() - lastTick;

        if (elapsedTime < targetFrameTime) {
            SDL_Delay(targetFrameTime - elapsedTime);
        }
        lastTick = currentTick;

        if(reachedEndBlock == 1){
            enChoix = true;
            sortieGameLoop = 1;
            printf("je suis ici\n");
            for (int i = 0; i < 11; i++)
                free(blockMap[i]);

            free(blockMap);

            free(zoneCollision);
            printf("je suis plus ici\n");  
        }
    }

// end:
//     printf("je suis dans le end\n");
//     for (int i = 0; i < 11; i++)
//         free(blockMap[i]);

//     free(blockMap);

//     free(zoneCollision);
//     // SDL_DestroyRenderer(renderer);
//     // SDL_DestroyWindow(window);
//     // SDL_Quit();

//     return 0;
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

    //création des boutons pour se deplacer dans les menus
    Button buttonMenuToShop = createButton(renderer, "sprites/buttonShop.png", 350, 250, 100, 100);
    buttonMenuToShop.hoverTexture = loadHoverTexture(renderer, "sprites/buttonShopHover.png"); 

    Button buttonShopToMenu = createButton(renderer, "sprites/button1.png", 20, 20, 150, 60);
    buttonShopToMenu.hoverTexture = loadHoverTexture(renderer, "sprites/buttonBackHover.png"); 

    Button buttonMenuToChoice = createButton(renderer, "sprites/buttonPlay.png", 550, 250, 100, 100);
    buttonMenuToChoice.hoverTexture = loadHoverTexture(renderer, "sprites/buttonChoiceHover.png"); 

    Button buttonChoiceToMenu = createButton(renderer, "sprites/button1.png", 20, 20, 150, 60);
    buttonChoiceToMenu.hoverTexture = loadHoverTexture(renderer, "sprites/buttonBackHover.png"); 

    Button buttonMapToChoice = createButton(renderer, "sprites/button1.png", 830, 20, 150, 60);

    //création des boutons pour le shop
    Button cube1 = createButton(renderer, "sprites/cube1.png", 300, 200, 80, 80);
    Button cube2 = createButton(renderer, "sprites/cube2.png", 450, 200, 80, 80);
    Button cube3 = createButton(renderer, "sprites/cube3.png", 600, 200, 80, 80);
    Button cube4 = createButton(renderer, "sprites/cube4.png", 300, 350, 80, 80);
    Button cube5 = createButton(renderer, "sprites/cube5.png", 450, 350, 80, 80);
    Button cube6 = createButton(renderer, "sprites/cube6.png", 600, 350, 80, 80);

    //création des boutons pour les map
    Button buttonMap1 = createButton(renderer, "sprites/gentleGlide.png", 100, 200, 200, 200);
    buttonMap1.hoverTexture = loadHoverTexture(renderer, "sprites/gentleGlideHover.png"); 

    Button buttonMap2 = createButton(renderer, "sprites/neverGiveUp.png", 400, 200, 200, 200);
    buttonMap2.hoverTexture = loadHoverTexture(renderer, "sprites/neverGiveUpHover.png"); 

    Button buttonMap3 = createButton(renderer, "sprites/eternalAscension.png", 700, 200, 200, 200);
    buttonMap3.hoverTexture = loadHoverTexture(renderer, "sprites/eternalAscensionHover.png"); 


    //création des textures au moment ou on survole un bouton
    bool ButtonMenuToShopHovered = false;
    bool ButtonMenuToChoiceHovered = false;
    bool ButtonShopToMenuHovered = false;
    bool ButtonChoiceToMenuHovered = false;

    bool neverGiveUpHovered = false;
    bool gentleGlideHovered = false;
    bool eternalAscensionHovered = false;

    char** map = malloc(sizeof(char*) * 11);
    
    for(int i = 0; i < 11; i++)
        map[i] = malloc(sizeof(char) * 1000);

   while (!terminer) {
        SDL_RenderClear(renderer);

        if (enMenu && !enShop && !enChoix) { //menu principal
            SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
            SDL_Texture* currentButtonMenuToShopTexture = ButtonMenuToShopHovered ? buttonMenuToShop.hoverTexture : buttonMenuToShop.texture;
            SDL_RenderCopy(renderer, currentButtonMenuToShopTexture, NULL, &buttonMenuToShop.rect);
            SDL_Texture* currentButtonMenuToChoiceTexture = ButtonMenuToChoiceHovered ? buttonMenuToChoice.hoverTexture : buttonMenuToChoice.texture;
            SDL_RenderCopy(renderer, currentButtonMenuToChoiceTexture, NULL, &buttonMenuToChoice.rect);

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
                        case SDL_MOUSEMOTION:
                            int mouseX = evenements.motion.x;
                            int mouseY = evenements.motion.y;
                            ButtonMenuToShopHovered = isMouseOnButton(mouseX, mouseY, buttonMenuToShop.rect);
                            ButtonMenuToChoiceHovered = isMouseOnButton(mouseX, mouseY, buttonMenuToChoice.rect);
                        break;
                        case SDL_MOUSEBUTTONDOWN:
                            mouseX = evenements.motion.x;
                            mouseY = evenements.motion.y;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            //bouton pour aller au shop
                            if (isMouseOnButton(mouseX, mouseY, buttonMenuToShop.rect)) {
                                enShop = true;
                                enMenu = false;
                                enChoix = false;
                            }
                            //bouton pour aller au choix du lvl
                            if (isMouseOnButton(mouseX, mouseY, buttonMenuToChoice.rect)) {
                                enShop = false;
                                enMenu = false;
                                enChoix = true;
                            }
                            break;
                }
            }
        } else if (!enMenu && enShop && !enChoix){ //dans le shop
            SDL_RenderCopy(renderer,menuShopTexture, NULL, NULL);
            SDL_Texture* currentButtonShopToMenuTexture = ButtonShopToMenuHovered ? buttonShopToMenu.hoverTexture : buttonShopToMenu.texture;
            SDL_RenderCopy(renderer, currentButtonShopToMenuTexture, NULL, &buttonShopToMenu.rect);            
            SDL_RenderCopy(renderer, cube1.texture, NULL, &cube1.rect);
            SDL_RenderCopy(renderer, cube2.texture, NULL, &cube2.rect);
            SDL_RenderCopy(renderer, cube3.texture, NULL, &cube3.rect);
            SDL_RenderCopy(renderer, cube4.texture, NULL, &cube4.rect);
            SDL_RenderCopy(renderer, cube5.texture, NULL, &cube5.rect);
            SDL_RenderCopy(renderer, cube6.texture, NULL, &cube6.rect);

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
                    case SDL_MOUSEMOTION:
                            int mouseX = evenements.motion.x;
                            int mouseY = evenements.motion.y;
                            ButtonShopToMenuHovered = isMouseOnButton(mouseX, mouseY, buttonShopToMenu.rect);
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        mouseX = evenements.motion.x;
                        mouseY = evenements.motion.y;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        //bouton retour en arriere
                        if (isMouseOnButton(mouseX, mouseY, buttonShopToMenu.rect)) {
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
            SDL_Texture* currentButtonChoiceToMenuTexture = ButtonChoiceToMenuHovered ? buttonChoiceToMenu.hoverTexture : buttonChoiceToMenu.texture;
            SDL_RenderCopy(renderer, currentButtonChoiceToMenuTexture, NULL, &buttonChoiceToMenu.rect);

            SDL_Texture* currentButtonChoiceMap1Texture = gentleGlideHovered ? buttonMap1.hoverTexture : buttonMap1.texture;
            SDL_RenderCopy(renderer, currentButtonChoiceMap1Texture, NULL, &buttonMap1.rect);

            SDL_Texture* currentButtonChoiceMap2Texture = neverGiveUpHovered ? buttonMap2.hoverTexture : buttonMap2.texture;
            SDL_RenderCopy(renderer, currentButtonChoiceMap2Texture, NULL, &buttonMap2.rect);

            SDL_Texture* currentButtonChoiceMap3Texture = eternalAscensionHovered ? buttonMap3.hoverTexture : buttonMap3.texture;
            SDL_RenderCopy(renderer, currentButtonChoiceMap3Texture, NULL, &buttonMap3.rect);

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
                    case SDL_MOUSEMOTION:
                            int mouseX = evenements.motion.x;
                            int mouseY = evenements.motion.y;
                            ButtonChoiceToMenuHovered = isMouseOnButton(mouseX, mouseY, buttonChoiceToMenu.rect);
                            gentleGlideHovered = isMouseOnButton(mouseX, mouseY, buttonMap1.rect);
                            neverGiveUpHovered = isMouseOnButton(mouseX, mouseY, buttonMap2.rect);
                            eternalAscensionHovered  = isMouseOnButton(mouseX, mouseY, buttonMap3.rect);
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                            mouseX = 0;
                            mouseY = 0;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            //bouton retour en arriere
                            if (isMouseOnButton(mouseX, mouseY, buttonChoiceToMenu.rect)) {
                                enShop = false;
                                enMenu = true;
                                enChoix = false;
                            }
                            //bouton des choix de lvl
                            if (isMouseOnButton(mouseX, mouseY, buttonMap1.rect)) {
                                getMap(map, "map");
                                enShop = false;
                                enMenu = false;
                                enChoix = false;
                                printf("je sors du choix de lvl\n");
                            }
                            if (isMouseOnButton(mouseX, mouseY, buttonMap2.rect)) {
                                getMap(map, "map2");
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
            printf("fin de gameloop\n");
            enChoix = true;
            // for (int i = 0; i < 11; i++)
            //     free(map[i]);
            // free(map);

           
        }

        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(menuTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    IMG_Quit();
    freeButtons(&buttonMenuToShop, 1);
    freeButtons(&buttonShopToMenu, 1);
    freeButtons(&buttonMenuToChoice, 1);
    freeButtons(&buttonChoiceToMenu, 1);
    freeButtons(&cube1, 1);
    freeButtons(&cube2, 1);
    freeButtons(&cube3, 1);
    freeButtons(&cube4, 1);
    freeButtons(&cube5, 1);
    freeButtons(&cube6, 1);
    freeButtons(&buttonMapToChoice, 1);
    freeButtons(&buttonMap1, 1);
    freeButtons(&buttonMap2, 1);
    freeButtons(&buttonMap3, 1);

    printf("1\n");
    return 0;
}