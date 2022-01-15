#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "utility.h"
#include "shared.h"
#include "game.h"

typedef struct props {
	RecArea body;
	ALLEGRO_BITMAP* img;
	bool isUse;
}Props;

Props props_create(float, float, float, float, const char*);
void drawProps(Props props);

#endif
