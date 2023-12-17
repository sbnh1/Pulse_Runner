#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "structs.h"

// fonction qui renvoie si la list est vide ou non
int estVide(List* L) {
    return L == NULL;
}

// fonctionqui renvoie le reste de la list
List* next(List* L) {
    if (L != NULL)
        return L->next;
    return NULL;
}

//fonction qui renvoie une list vide 
List* consVide() {
    return NULL;
}

// fonction qui renvoie le premier element de la list
Skin* premier(List* L) {
    if (L != NULL) 
        return &(L->skin);
    return NULL;
}

// constructeur de la list
List* consList() {
    return NULL;
}

// fonction qui ajoute un element a la list
List* ajouter(List* L, Skin* skin) {
    List* newList = (List*)malloc(sizeof(List));
    if (newList == NULL)
        return L;
    newList->skin = *skin;
    newList->next = L;
    return newList;
}

//fonction qui libere la memoiure de la list
void freeList(List* L) {
    if (L != NULL) {
        freeList(L->next);
        free(L);
    }
}

//fonction qui initialise les données dans la structur Skin
Skin* consSkin(char* path) {
    Skin* skin = (Skin*)malloc(sizeof(Skin));
    if (skin == NULL)
        return NULL;

    skin->path = (char*)malloc(strlen(path) + 1);
    if (skin->path == NULL) {
        free(skin);
        return NULL;
    }
    
    strcpy(skin->path, path);
    skin->isUnblocked = 0;

    return skin;
}

// fonction qui cherche dans la list et met a jour le parametre .isUnblocked de la structur Skin
void setUnblocked(List* L, char* path) {
    if (L == NULL)
        return;
    if (strcmp(L->skin.path, path) == 0)
        L->skin.isUnblocked = 1;
    setUnblocked(L->next, path);
}

// fonction que cherche un skin dans la list en fonction du path de son image 
int checkSkin(List* L, char* path) {
    if (L == NULL)
        return 0;
    if (strcmp(premier(L)->path, path) == 0)
        return premier(L)->isUnblocked;
    return checkSkin(next(L), path);
}