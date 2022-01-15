#define _CRT_SECURE_NO_WARNINGS

#include "scene_game_object.h"
#include "utility.h"
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Props props_create(float x, float y, float w, float h, const char* img_path) {
	Props props;
	memset(&props, 0, sizeof(props));

	props.img = load_bitmap(img_path);
	if (!props.img) {
		game_log("failed loading props image!");
	}

	props.body.x = x;
	props.body.y = y;
	props.body.w = w;
	props.body.h = h;

	props.isUse = false;

	return props;
}

void drawProps(Props props) {
	if (props.isUse) {
		return;
	}

	al_draw_scaled_bitmap(
		props.img,
		0, 0,
		al_get_bitmap_width(props.img), al_get_bitmap_height(props.img),
		props.body.x, props.body.y,
		props.body.w, props.body.h, 0
	);
}
