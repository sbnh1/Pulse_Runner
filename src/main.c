#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#include "structs.h"
#include "utils.h"
#include "fonctions_SDL.h"
#include "list.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600

#define TILE_SIZE 500
#define SCROLL_SPEED 1

#define SETZONECOLLISION(zC, map, z) \
    zC[0] = map[(z.y / 50)][(z.x / 50)]; \
    zC[1] = map[(z.y / 50)][(z.x / 50) + 1]; \
    zC[2] = map[(z.y / 50) + 1][(z.x / 50)]; \
    zC[3] = map[(z.y / 50) + 1][(z.x / 50) + 1];

#define GRAVITY 0.34
#define JUMP_SPEED -9

//initilisation de toutes les variables
bool enMenu = true;
bool enShop = false;
bool enChoix = false;
bool enFin = true;
bool terminer = false;
char spriteCube[50] = "sprites/cube1.png";
char mapPlayed[50] = "map1";

void gameLoop(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* imageSurface, SDL_Texture* imageTexture, SDL_Event event, Block** blockMap) {
    printf("%s\n", mapPlayed);
    SDL_Surface *backgroundSurface = IMG_Load("sprites/background.png");
    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);

    SDL_SetTextureColorMod(backgroundTexture, 128, 0, 128);

    SDL_Rect basePlayerRect = {200, 150, 50, 50};
    int sortieGameLoop = 0; 

    Player player = {
        .rect = basePlayerRect, // player rect 
        .speedX = 3,            // player speed
        .speedY = 0.0,          //player speed y
        .state = 0,             // state
        .sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load(spriteCube)) // player texture
    };

    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; // definition du SDL_Rect de la camera suivant le player 

    int targetFrameTime = 15;

    int elapsedTime;
    Uint32 lastTick = SDL_GetTicks();
    Uint32 currentTick = SDL_GetTicks();

    Block* zoneCollision = malloc(sizeof(Block) * 4);

    SDL_Rect zone = getZoneRect(player.rect);

    SDL_Rect lastPlayerPos = {0,0,0,0};

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
                    case SDLK_m: // retour au menu du choix de niveau
                        enShop = false;
                        enChoix = true;
                        enMenu = false;
                        enFin = false;
                        sortieGameLoop = 1;
                        break;
                }
            }
        }

        player.speedY += GRAVITY; // application de la gravité

        int collision;
        int reachedEndBlock = 0;

        // detection des colision sur l'axe des X
        zone = getZoneRect(player.rect);
        SETZONECOLLISION(zoneCollision, blockMap, zone)
        player.rect.x += player.speedX;
        collision = checkCollision(player.rect, zoneCollision);
        switch(collision){
            case '1': player.rect.x -= player.speedX; break;
            case '2': player.rect = basePlayerRect; break;
            case '3': reachedEndBlock = 1; break;
        } 

        // detection des colision sur l'axe des Y
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

        if(player.rect.x == lastPlayerPos.x && player.rect.y == lastPlayerPos.y) // si le block est immobile alors il meure
            player.rect = basePlayerRect;

        camera.x = player.rect.x - 300; // mise a jour de la position de la camera en fonction du joueur

        scrollOffset += SCROLL_SPEED;
        if (scrollOffset >= TILE_SIZE)
            scrollOffset = 0;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderBackground(renderer, backgroundTexture, scrollOffset, SCREEN_WIDTH, SCREEN_HEIGHT, TILE_SIZE); // rendu du background

        // boucle que affiche les uniquement les block visible en fonction de la position de la camera
        for (int i = 0; i < 11; i++) {
            for (int j = camera.x / 50; j < (camera.x + camera.w + 50) / 50; j++) {
                SDL_Rect* relativBlockPos;
                switch (blockMap[i][j].type) {
                    case '1':
                    case '2':
                    case '3':
                        relativBlockPos = relativPos(&camera, &blockMap[i][j].rect); // recupere la positon relativ du block par rapport a la position de la camera
                        SDL_RenderCopy(renderer, blockMap[i][j].sprite_image, NULL, relativBlockPos);
                        free(relativBlockPos);
                        break;
                }
            }
        }

        SDL_Rect* relativPlayerPos = relativPos(&camera, &player.rect); // recupere la positon relativ du player par rapport a la position de la camera
        SDL_RenderCopy(renderer, player.sprite_image, NULL, relativPlayerPos);
        free(relativPlayerPos);
        SDL_RenderPresent(renderer);

        lastPlayerPos = player.rect;

        currentTick = SDL_GetTicks();
        elapsedTime = SDL_GetTicks() - lastTick;

        if (elapsedTime < targetFrameTime) {
            SDL_Delay(targetFrameTime - elapsedTime);
        }
        lastTick = currentTick;

        if(reachedEndBlock == 1){
            enFin = true;
            sortieGameLoop = 1;
            updateDataFile(mapPlayed);
        }
    }
    free(zoneCollision);
    SDL_DestroyTexture(player.sprite_image);
    freeMap(blockMap);
    SDL_FreeSurface(backgroundSurface);
}

int main(int argc, char** argv) {
    SDL_Event evenements;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // Gestion de l'erreur d'initialisation de SDL
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
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

    // Initialisation de ttf
    if (TTF_Init() < 0) {
        // Gestion de l'erreur d'initialisation de SDL_ttf
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    TTF_Font *font = TTF_OpenFont("font/gintama.ttf",70);
    SDL_Color color = {0,191,255,255};
    char msg[] = "NIVEAU FINI";

    SDL_Texture* texte = charger_texte(msg,renderer,font,color);
    int texteH, texteW;
    SDL_QueryTexture(texte, NULL, NULL, &texteW, &texteH);  
    SDL_Rect text_pos; 
    text_pos.x = 300;
    text_pos.y = 100;
    text_pos.w = texteW; 
    text_pos.h = texteH;    

    //initialisation du texte
    char msgChoix[] = "VOUS AVEZ FINI LE NIVEAU AVEC CE CUBE : ";
    SDL_Texture* texteChoix = charger_texte(msgChoix,renderer,font,color);
    int texteHChoix, texteWChoix;
    SDL_QueryTexture(texte, NULL, NULL, &texteWChoix, &texteHChoix);  
    SDL_Rect text_posChoix; 
    text_posChoix.x = 280;
    text_posChoix.y = 400;
    text_posChoix.w = texteWChoix; 
    text_posChoix.h = texteHChoix;    
    char msgUnblocked[] = "VOUS AVEZ DEBLOQUE LE SKIN SUIVANT : ";
    SDL_Texture* texteUnblocked = charger_texte(msgUnblocked,renderer,font,color);
    int texteHUnblocked, texteWUnblocked;
    SDL_QueryTexture(texte, NULL, NULL, &texteWUnblocked, &texteHUnblocked);  
    SDL_Rect text_posUnblocked; 
    text_posUnblocked.x = 280;
    text_posUnblocked.y = 500;
    text_posUnblocked.w = texteWUnblocked; 
    text_posUnblocked.h = texteHUnblocked;  

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

    /* FIN DE NIVEAU */
    SDL_Surface* finMapSurface = IMG_Load("sprites/finDeNiveau.png");  
    if (!finMapSurface) {
        SDL_Log("Erreur lors du chargement de l'image : %s", IMG_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    SDL_Texture* finMapTexture = SDL_CreateTextureFromSurface(renderer, finMapSurface);  
    SDL_FreeSurface(finMapSurface);

    if (!finMapTexture) {
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

    List* skinsList = consList();

    skinsList = ajouter(skinsList, consSkin("sprites/cube1.png"));
    skinsList = ajouter(skinsList, consSkin("sprites/cube2.png"));
    skinsList = ajouter(skinsList, consSkin("sprites/cube3.png"));
    skinsList = ajouter(skinsList, consSkin("sprites/cube4.png"));
    skinsList = ajouter(skinsList, consSkin("sprites/cube5.png"));
    skinsList = ajouter(skinsList, consSkin("sprites/cube6.png"));

    setUnblocked(skinsList, "sprites/cube1.png");
    setUnblocked(skinsList, "sprites/cube2.png");
    setUnblocked(skinsList, "sprites/cube3.png");

    //création des boutons pour le choix du skin
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

    //booléen qui s'occupe de savoir si on survole ou non un boutton
    bool ButtonMenuToShopHovered = false;
    bool ButtonMenuToChoiceHovered = false;
    bool ButtonShopToMenuHovered = false;
    bool ButtonChoiceToMenuHovered = false;
    bool neverGiveUpHovered = false;
    bool gentleGlideHovered = false;
    bool eternalAscensionHovered = false;

    Block** map;

    int* mapData;

    // variable pour savoir si on a débloqué un cube ou non
    mapData = lireData("data");
    int map1unblocked = mapData[0];
    int map2unblocked = mapData[1];
    int map3unblocked = mapData[2];
    free(mapData);

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
                    case SDL_KEYDOWN: //action lorsque l'on appuie sur une touche
                        if (evenements.key.keysym.sym == SDLK_q || evenements.key.keysym.sym == SDLK_ESCAPE) {
                            terminer = true;
                        }
                        break;
                        case SDL_MOUSEMOTION: //action lorsque l'on passe la souris au dessus d'un bouton
                            int mouseX = evenements.motion.x;
                            int mouseY = evenements.motion.y;
                            ButtonMenuToShopHovered = isMouseOnButton(mouseX, mouseY, buttonMenuToShop.rect);
                            ButtonMenuToChoiceHovered = isMouseOnButton(mouseX, mouseY, buttonMenuToChoice.rect);
                        break;
                        case SDL_MOUSEBUTTONDOWN: //action lorsque l'on appuie sur un bouton
                            mouseX = evenements.motion.x;
                            mouseY = evenements.motion.y;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            //bouton pour aller au shop
                            if (isMouseOnButton(mouseX, mouseY, buttonMenuToShop.rect)) {
                                enShop = true;
                                enMenu = false;
                            }
                            //bouton pour aller au choix du lvl
                            if (isMouseOnButton(mouseX, mouseY, buttonMenuToChoice.rect)) {
                                enMenu = false;
                                enChoix = true;
                            }
                            break;
                }
            }
        } else if (!enMenu && enShop && !enChoix){ //dans le shop la ou on peut choisir le skin du cube
            //débloquage des skin
            mapData = lireData("data");
            if(mapData[0] == 1)
                setUnblocked(skinsList, "sprites/cube4.png");
            if(mapData[1] == 1)
                setUnblocked(skinsList, "sprites/cube5.png");
            if(mapData[2] == 1)
                setUnblocked(skinsList, "sprites/cube6.png");

            free(mapData);
            SDL_RenderCopy(renderer,menuShopTexture, NULL, NULL);
            SDL_Texture* currentButtonShopToMenuTexture = ButtonShopToMenuHovered ? buttonShopToMenu.hoverTexture : buttonShopToMenu.texture;
            SDL_RenderCopy(renderer, currentButtonShopToMenuTexture, NULL, &buttonShopToMenu.rect); 
            //affichage du skin si il est débloqué        
            if(checkSkin(skinsList, "sprites/cube1.png")) 
                SDL_RenderCopy(renderer, cube1.texture, NULL, &cube1.rect);
            if(checkSkin(skinsList, "sprites/cube2.png"))
                SDL_RenderCopy(renderer, cube2.texture, NULL, &cube2.rect);
            if(checkSkin(skinsList, "sprites/cube3.png"))
                SDL_RenderCopy(renderer, cube3.texture, NULL, &cube3.rect);
            if(checkSkin(skinsList, "sprites/cube4.png"))
                SDL_RenderCopy(renderer, cube4.texture, NULL, &cube4.rect);
            if(checkSkin(skinsList, "sprites/cube5.png"))
                SDL_RenderCopy(renderer, cube5.texture, NULL, &cube5.rect);
            if(checkSkin(skinsList, "sprites/cube6.png"))
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
                        }
                        if(isMouseOnButton(mouseX,mouseY,cube1.rect)){
                            strcpy(spriteCube, "sprites/cube1.png"); // attribution de la bonne image pour le cube
                        }
                        if(isMouseOnButton(mouseX,mouseY,cube2.rect)){
                            strcpy(spriteCube, "sprites/cube2.png");
                        }
                        if(isMouseOnButton(mouseX,mouseY,cube3.rect)){
                            strcpy(spriteCube, "sprites/cube3.png");
                        }
                        if(isMouseOnButton(mouseX,mouseY,cube4.rect)){
                            strcpy(spriteCube, "sprites/cube4.png");
                        }
                        if(isMouseOnButton(mouseX,mouseY,cube5.rect)){
                            strcpy(spriteCube, "sprites/cube5.png");
                        }
                        if(isMouseOnButton(mouseX,mouseY,cube6.rect)){
                            strcpy(spriteCube, "sprites/cube6.png");
                        }
                    break;
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

            //fonction pour simplifié le code, elle permet de savoir si on a fini la map ou si on a quitter la map et afficher la bonne page en conséquence
            void EndOrNotEnd(){
                if (enFin == true) {
                    enShop = false;
                    enMenu = false;
                    enChoix = false;
                }
            }

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
                                enMenu = true;
                                enChoix = false;
                            }
                            //bouton des choix de lvl
                            if (isMouseOnButton(mouseX, mouseY, buttonMap1.rect)){
                                strcpy(mapPlayed, "map1");
                                gameLoop(window, renderer, imageSurface, imageTexture, evenements, getMap("maps/map1", renderer));
                                EndOrNotEnd();
                            } else if (isMouseOnButton(mouseX, mouseY, buttonMap2.rect)) {
                                strcpy(mapPlayed, "map2");
                                gameLoop(window, renderer, imageSurface, imageTexture, evenements, getMap("maps/map2", renderer));
                                EndOrNotEnd();
                            } else if (isMouseOnButton(mouseX, mouseY, buttonMap3.rect)){
                                strcpy(mapPlayed, "map3");
                                gameLoop(window, renderer, imageSurface, imageTexture, evenements, getMap("maps/map3", renderer));
                                EndOrNotEnd();
                            }
                    break;

                }
            }
        } else if (!enMenu && !enShop && !enChoix){ //Affichage quand on a fini une map
            SDL_RenderCopy(renderer, finMapTexture, NULL, NULL);
            SDL_RenderCopy(renderer, texte,NULL,&text_pos);
            SDL_RenderCopy(renderer,texteChoix, NULL, &text_posChoix);
            SDL_Texture* currentButtonMenuToShopTexture = ButtonMenuToShopHovered ? buttonMenuToShop.hoverTexture : buttonMenuToShop.texture;
            SDL_RenderCopy(renderer, currentButtonMenuToShopTexture, NULL, &buttonMenuToShop.rect);
            SDL_Texture* currentButtonMenuToChoiceTexture = ButtonMenuToChoiceHovered ? buttonMenuToChoice.hoverTexture : buttonMenuToChoice.texture;
            SDL_RenderCopy(renderer, currentButtonMenuToChoiceTexture, NULL, &buttonMenuToChoice.rect);

            //affichage du cube avec le quel on a fini la mapp
            SDL_Texture* cubeSprite = SDL_CreateTextureFromSurface(renderer, IMG_Load(spriteCube));
            SDL_Rect cubeRect = {SCREEN_WIDTH - 250, SCREEN_HEIGHT - 200, 75, 75}; 
            SDL_RenderCopy(renderer, cubeSprite, NULL, &cubeRect);
            SDL_Rect cubeRectUnlocked = {SCREEN_WIDTH - 250, SCREEN_HEIGHT - 100, 75, 75}; 

            //affichage du cube qu'on a débloqué quand on en débloque un
            mapData = lireData("data");
            if(map1unblocked != (int)mapData[0]){
                SDL_RenderCopy(renderer,texteUnblocked, NULL, &text_posUnblocked);
                char spriteCubeUnblocked[50] = "sprites/cube4.png";
                SDL_Texture* cubeSpriteUnblocked = SDL_CreateTextureFromSurface(renderer, IMG_Load(spriteCubeUnblocked));
                SDL_RenderCopy(renderer, cubeSpriteUnblocked, NULL, &cubeRectUnlocked);
                SDL_DestroyTexture(cubeSpriteUnblocked);
            }
            if(map2unblocked != (int)mapData[1]){
                SDL_RenderCopy(renderer,texteUnblocked, NULL, &text_posUnblocked);
                char spriteCubeUnblocked[50] = "sprites/cube5.png";
                SDL_Texture* cubeSpriteUnblocked = SDL_CreateTextureFromSurface(renderer, IMG_Load(spriteCubeUnblocked));
                SDL_RenderCopy(renderer, cubeSpriteUnblocked, NULL, &cubeRectUnlocked);
                SDL_DestroyTexture(cubeSpriteUnblocked);
            }
            if(map3unblocked != (int)mapData[2]){
                SDL_RenderCopy(renderer,texteUnblocked, NULL, &text_posUnblocked);
                char spriteCubeUnblocked[50] = "sprites/cube5.png";
                SDL_Texture* cubeSpriteUnblocked = SDL_CreateTextureFromSurface(renderer, IMG_Load(spriteCubeUnblocked));
                SDL_RenderCopy(renderer, cubeSpriteUnblocked, NULL, &cubeRectUnlocked);
                SDL_DestroyTexture(cubeSpriteUnblocked);
            }
            free(mapData);
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
                        ButtonMenuToShopHovered = isMouseOnButton(mouseX, mouseY, buttonMenuToShop.rect);
                        ButtonMenuToChoiceHovered = isMouseOnButton(mouseX, mouseY, buttonMenuToChoice.rect);
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        mouseX = evenements.motion.x;
                        mouseY = evenements.motion.y;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        if (isMouseOnButton(mouseX, mouseY, buttonMenuToShop.rect)) {
                            enShop = true;
                        }
                        if (isMouseOnButton(mouseX, mouseY, buttonMenuToChoice.rect)) {
                            enChoix = true;
                        }
                    break;
                }
            }
            SDL_DestroyTexture(cubeSprite);
        }

        SDL_RenderPresent(renderer);
    }
    //libération de toute les ressources quand on ferme le jeu
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(fond);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
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
    freeButtons(&buttonMap1, 1);
    freeButtons(&buttonMap2, 1);
    freeButtons(&buttonMap3, 1);
    SDL_Quit();
    return 0;
}