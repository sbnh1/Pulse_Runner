#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "structs.h"

int estVide(List* L) {
    return L == NULL;
}

List* next(List* L) {
    if (L != NULL)
        return L->next;
    return NULL;
}

List* consVide() {
    return NULL;
}

Skin* premier(List* L) {
    if (L != NULL) 
        return &(L->skin);
    return NULL;
}

List* consList() {
    return NULL;
}

List* ajouter(List* L, Skin* skin) {
    List* newList = (List*)malloc(sizeof(List));
    if (newList == NULL)
        return L;
    newList->skin = *skin;
    newList->next = L;
    return newList;
}

void freeList(List* L) {
    if (L != NULL) {
        freeList(L->next);
        free(L);
    }
}

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

void setUnblocked(List* L, char* path) {
    if (L == NULL)
        return;
    if (strcmp(L->skin.path, path) == 0)
        L->skin.isUnblocked = 1;
    setUnblocked(L->next, path);
}

int checkSkin(List* L, char* path) {
    if (L == NULL)
        return 0;
    if (strcmp(premier(L)->path, path) == 0)
        return premier(L)->isUnblocked;
    return checkSkin(next(L), path);
}