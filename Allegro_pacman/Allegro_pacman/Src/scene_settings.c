#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_button_object.h"
#include "scene_settings.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"

/* Internal Variables*/
static ALLEGRO_SAMPLE_ID menuBGM;

static Button btn_volume_up;
static Button btn_volume_down;

static void init() {

	btn_volume_up = button_create(450, 400, 50, 50, "Assets/volume_up.png", "Assets/volume_up1.png");
	btn_volume_down = button_create(300, 400, 50, 50, "Assets/volume_down.png", "Assets/volume_down1.png");

	stop_bgm(menuBGM);
	menuBGM = play_bgm(themeMusic, music_volume);
}


static void draw() {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"PRESS \"ENTER\" TO BACK TO MENU"
	);

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		400, 350,
		ALLEGRO_ALIGN_CENTER,
		"VOLUME"
	);

	al_draw_textf(
		menuFont,
		al_map_rgb(255, 255, 255),
		390, 410,
		ALLEGRO_ALIGN_LEFT,
		"%d",
		(int)(music_volume * 10)
	);

	// Draw button
	drawButton(btn_volume_up);
	drawButton(btn_volume_down);
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	// Update button's status(hovered), and utilize the function `pnt_in_rect`.
	btn_volume_up.hovered = buttonHover(btn_volume_up, mouse_x, mouse_y);
	btn_volume_down.hovered = buttonHover(btn_volume_down, mouse_x, mouse_y);
}

static void on_mouse_down() {
	if (btn_volume_up.hovered) {
		if (music_volume < 1.0) {
			music_volume += 0.1;
			effect_volume += 0.1;
			// game_log("volume up\n");

			stop_bgm(menuBGM);
			menuBGM = play_bgm(themeMusic, music_volume);
		}
	}
	else if (btn_volume_down.hovered) {
		if (music_volume >= 0.1) {
			music_volume -= 0.1;
			effect_volume -= 0.1;
			// game_log("volume down\n");

			stop_bgm(menuBGM);
			menuBGM = play_bgm(themeMusic, music_volume);
		}
	}
}


static void destroy() {
	stop_bgm(menuBGM);

	// Destroy buttons' images
	al_destroy_bitmap(btn_volume_up.default_img);
	al_destroy_bitmap(btn_volume_up.hovered_img);
	al_destroy_bitmap(btn_volume_down.default_img);
	al_destroy_bitmap(btn_volume_down.hovered_img);
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
Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;

	game_log("Settings scene created");
	return scene;
}