#ifndef LIST_H
#define LIST_H

#include "structs.h"

int estVide(List* L);
List* next(List* L);
List* consVide();
Skin* premier(List* L);
List* consList();
List* ajouter(List* L, Skin* skin);
void freeList(List* L);

Skin* consSkin(char* path);
void setUnblocked(List* L, char* path);
int checkSkin(List* L, char* path);

#endif