#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "structs.h"
#include "utils.h"

#include <SDL2/SDL.h>

// fonction pour editer le fichier data
void updateDataFile(char* map) {
    FILE *file;
    char buffer[100];

    file = fopen("data", "r+");// ouvre le fichier en mode lecture ecriture

    // boucle qui lit le fichier ligne par ligne
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char currentMap[50];
        int value;

        // extraction et verification du nom de la map
        if (sscanf(buffer, "%s %d", currentMap, &value) == 2) {
            if (strcmp(currentMap, map) == 0) {
                fseek(file, -strlen(buffer), SEEK_CUR);
                //mise a jour de la valeur pour une map donné
                fprintf(file, "%s 1\n", currentMap);
            }
        }
    }

    fclose(file); // fermeture du fichier 
}

// fonction pour lire le fichier data
int* lireData(const char* file) {
    FILE* fichier = fopen(file, "r");
    
    int* tab = (int*)malloc(3 * sizeof(int));

    for (int i = 0; i < 3; i++) {
        char nomMap[10];
        int val;

        if (fscanf(fichier, "%s %d", nomMap, &val) == 2)
            tab[i] = val;
    }

    fclose(fichier);

    return tab;
}

// fonction pour liberer la memoire de blockmap
void freeMap(Block** blockMap){
    for (int i = 0; i < 11; i++){
        for (int j = 0; j < 1000; j++){
            if(blockMap[i][j].sprite_image != NULL)
                SDL_DestroyTexture(blockMap[i][j].sprite_image);
            blockMap[i][j].sprite_image = NULL;
        }
        free(blockMap[i]);        
    }
    free(blockMap);
}

// fonction qui renvoie avec quel block le player est en collision si il n'est pas en collision alors la fonction renvoie -1
int checkCollision(SDL_Rect rect1, Block * zone) {
    for (int i = 0; i < 4; i++){
        if(SDL_HasIntersection(&rect1, &zone[i].hitBox)) 
            return zone[i].type;
    }
    return -1;
}

// fonction qui prend un fichier en entré et qui le converti en map sous forme d'un tableau 2D de Blocks
Block** getMap(char* fileName, SDL_Renderer *renderer){
    
    FILE* file = fopen(fileName, "r");

    char line[1000];

    //initialisation et malloc du tableau 2D de Blocks
    Block** blockMap = malloc(sizeof(Block*) * 11);
    for (int i = 0; i < 11; i++){
        blockMap[i] = malloc(sizeof(Block) * 1000);
        for(int j = 0; j < 1000; j++){
            blockMap[i][j].type = '0';
            blockMap[i][j].sprite_image = NULL;
        }
    }

    SDL_Rect blockRect;
    SDL_Rect hitBox;


    //initialisation des données dans le tableau qui definissent les caracteristiques du block
    for(int i = 0; fgets(line, sizeof(line), file) != NULL; i++){
        line[strcspn(line, "\n")] = '\0';
        if(strlen(line) > 2){
            for(int j = 0; j < strlen(line); j++){
                switch (line[j]) {      
                    case '1':
                        blockRect.x = j * 50;                   // initialisation du SDL_Rect du block definisant sa position 
                        blockRect.y = i * 50;                   // et sa taille, ainsi de hitbox qui est un autre SDL_Rect qui lui
                        blockRect.w = 50;                       // defini la zone ou le player pourra interagir avec le block
                        blockRect.h = 50;                       
                        blockMap[i][j].rect = blockRect;
                        blockMap[i][j].hitBox = blockRect;
                        blockMap[i][j].sprite_image = SDL_CreateTextureFromSurface(renderer, IMG_Load("sprites/block.png")); // definition de limage que portera le block 
                        blockMap[i][j].type = '1';
                        break;
                    case '2':
                        blockRect.x = j * 50;
                        blockRect.y = i * 50;
                        blockRect.w = 50;
                        blockRect.h = 50;
                        hitBox.x = blockRect.x + 20; 
                        hitBox.y = blockRect.y + 10;
                        hitBox.w = 10;
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
    }    
    fclose(file);
    return blockMap;
}

// renvoie la un rect deffinissant la position d'une zone autour du joueur ou on regarde les collision
SDL_Rect getZoneRect(SDL_Rect rect){
    SDL_Rect resRect = {
        rect.x-(rect.x%50),
        rect.y-(rect.y%50),
        100,
        100
    };
    return resRect;
}

// renvoie la position relative d'un SDL_Rect par rapport a un autre (cette fonction est utuliser pour la gestion de la camera)
SDL_Rect* relativPos(SDL_Rect* camera, SDL_Rect* player) {
    SDL_Rect* res = malloc(sizeof(SDL_Rect));
    res->x = player->x - camera->x;
    res->y = player->y - camera->y;
    res->w = player->w;
    res->h = player->h;

    return res;
}

// fonction qui gere laffichage du background du jeu
void renderBackground(SDL_Renderer *renderer, SDL_Texture *backgroundTexture, int scrollOffset, int SCREEN_WIDTH, int SCREEN_HEIGHT, int TILE_SIZE) {
    for (int y = 0; y < SCREEN_HEIGHT; y += TILE_SIZE) {
        for (int x = -SCREEN_WIDTH; x < SCREEN_WIDTH*2; x += TILE_SIZE) {
            SDL_Rect destRect = {x - scrollOffset, y, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
        }
    }
}