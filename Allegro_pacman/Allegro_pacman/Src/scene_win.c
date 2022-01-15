#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_settings.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"

/* Internal Variables*/
static ALLEGRO_BITMAP* pacman_win = NULL;
static ALLEGRO_SAMPLE_ID winBGM;
static int pacman_winSize;

static void init() {
	stop_bgm(winBGM);
	winBGM = play_bgm(PACMAN_WIN_SOUND, music_volume);
	pacman_win = load_bitmap("Assets/pacman_win.png");
	pacman_winSize = al_get_bitmap_width(pacman_win);
}


static void draw() {

	const float scale = 1.5;
	const float offset = (SCREEN_W >> 1) - 0.5 * scale * pacman_winSize;

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_scaled_bitmap(
		pacman_win,
		0, 0,
		pacman_winSize, pacman_winSize,
		offset, offset,
		pacman_winSize * scale, pacman_winSize * scale,
		0
	);

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		100,
		ALLEGRO_ALIGN_CENTER,
		"YOU WIN !!!"
	);

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"PRESS \"ENTER\" TO BACK TO MENU"
	);
}

static void destroy() {
	stop_bgm(winBGM);
	al_destroy_bitmap(pacman_win);
}

static void on_key_down(int keycode) {

	switch (keycode) {
	case ALLEGRO_KEY_ENTER:
		game_change_scene(scene_menu_create());
		break;
	default:
		break;
	}
}

// The only function that is shared across files.
Scene scene_win_create(void) {

	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Win";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;

	game_log("Win scene created");
	return scene;
}