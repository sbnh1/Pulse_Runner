#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fonctions_SDL.h"

int main(void)
{
    SDL_Window* fenetre;
    SDL_Renderer* ecran;
    SDL_Event evenements;
    bool terminer = false;

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("Erreur d’initialisation de la SDL: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    fenetre = SDL_CreateWindow("Fenetre SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_RESIZABLE);
    if(fenetre == NULL)
    {
        printf("Erreur de la creation d’une fenetre: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    ecran = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);

    // Charger l’image en fond
    SDL_Texture* fond = chargerTexture("sprites/fond.bmp", ecran);

    // Initialiser SDL_mixer
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Erreur d’initialisation de SDL_mixer: %s", Mix_GetError());
        SDL_DestroyTexture(fond);
        SDL_DestroyRenderer(ecran);
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Charger la musique
    Mix_Music* musique = Mix_LoadMUS("music/nujabes_arurian_dance.mp3");
    if(musique == NULL)
    {
        printf("Erreur de chargement de la musique: %s", Mix_GetError());
        // Gérer l'erreur comme vous le souhaitez
    }

    // Jouer la musique en boucle
    Mix_PlayMusic(musique, -1);

    // Boucle principale
    while (!terminer)
    {
        SDL_RenderClear(ecran);
        SDL_RenderCopy(ecran, fond, NULL, NULL);
        SDL_RenderPresent(ecran);

        SDL_PollEvent(&evenements);

        switch (evenements.type)
        {
        case SDL_QUIT:
            terminer = true;
            break;
        case SDL_KEYDOWN:
            switch (evenements.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            case SDLK_q:
                terminer = true;
                break;
            }
        }
    }

    // Libérer la mémoire
    Mix_FreeMusic(musique);
    Mix_CloseAudio();
    SDL_DestroyTexture(fond);
    SDL_DestroyRenderer(ecran);
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    return 0;
}
