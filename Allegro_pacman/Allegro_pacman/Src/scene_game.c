#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "scene_win.h"
#include "scene_settings.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"
#include "scene_game_object.h"

#define GHOST_NUM 4
/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
extern ALLEGRO_SAMPLE* PACMAN_POWER_UP_SOUND;
ALLEGRO_TIMER* power_up_timer;
ALLEGRO_TIMER* freeze_timer;
ALLEGRO_TIMER* speed_up_timer;
const int power_up_duration = 15;
const int freeze_duration = 300;
const int speed_up_duration = 300;

bool game_over = false;

/* Internal variables*/
static ALLEGRO_SAMPLE_ID PACMAN_POWER_UP_SOUND_ID;
static Pacman* pman;
static Map* basic_map;
static Ghost** ghosts;
static Props Freeze_item;
static Props SpeedUp_item;
static const int SpeedUp_speed = 4;

bool debug_mode = false;
bool cheat_mode = false;

/* Declare static function prototypes */
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(void);
static void render_init_screen(void);
static void draw_hitboxes(void);

static void init(void) {
	game_over = false;

	// create map
	basic_map = create_map("Assets/map_pacman.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}
	// create pacman
	pman = pacman_create();
	if (!pman) {
		game_abort("error on creating pacamn\n");
	}

	// Allocate dynamic memory for ghosts array.

	else {
		ghosts = (Ghost**)malloc(sizeof(Ghost*) * GHOST_NUM);
		if (!ghosts) {
			game_abort("error on allocate ghosts' dynamic memory.\n");
		}

		// create ghosts.
		GhostType ghost_type = Blinky;
		for (int i = 0; i < GHOST_NUM; i++) {
			// game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(ghost_type++);
			if (!ghosts[i])
				game_abort("error creating ghost\n");
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick / sec
	if (!power_up_timer)
		game_abort("Error on create timer\n");

	freeze_timer = al_create_timer(1.0f / 64);
	if (!freeze_timer)
		game_abort("Error on create timer\n");

	speed_up_timer = al_create_timer(1.0f / 64);
	if (!speed_up_timer)
		game_abort("Error on create timer\n");

	Freeze_item = props_create(700, 700, 40, 40, "Assets/ice.png");
	SpeedUp_item = props_create(650, 700, 40, 40, "Assets/speedup.png");

	return;
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;

	// check which item you are going to eat and use `pacman_eatItem` to deal with it.
	switch (basic_map->map[Grid_y][Grid_x])
	{
	case '.':
		pacman_eatItem(pman, '.');
		basic_map->map[Grid_y][Grid_x] = ' ';
		basic_map->beansCount += 1;
		basic_map->score += 10;
		break;
	case 'P':
		pacman_eatItem(pman, 'P');
		basic_map->map[Grid_y][Grid_x] = ' ';
		al_set_timer_count(power_up_timer, 0);
		al_start_timer(power_up_timer);
		stop_bgm(PACMAN_POWER_UP_SOUND_ID);
		PACMAN_POWER_UP_SOUND_ID = play_audio(PACMAN_POWER_UP_SOUND, music_volume);
		for (int i = 0; i < GHOST_NUM; i++) {
			ghost_toggle_FLEE(ghosts[i], true);
		}
		break;
	default:
		break;
	}
}
static void status_update(void) {

	if (al_get_timer_count(power_up_timer) > power_up_duration) {
		al_stop_timer(power_up_timer);
		stop_bgm(PACMAN_POWER_UP_SOUND_ID);
		pman->powerUp = false;
		for (int i = 0; i < GHOST_NUM; i++) {
			ghost_toggle_FLEE(ghosts[i], false);
		}
	}

	if (al_get_timer_count(freeze_timer) > freeze_duration) {
		al_stop_timer(freeze_timer);
		for (int i = 0; i < GHOST_NUM; i++) {
			ghost_toggle_FREEZE(ghosts[i], false);
		}
	}

	if (al_get_timer_count(speed_up_timer) > speed_up_duration) {
		al_stop_timer(speed_up_timer);
		pman->speed = pman->basic_speed;
	}

	for (int i = 0; i < GHOST_NUM; i++) {
		if (ghosts[i]->status == GO_IN)
			continue;

		if (!cheat_mode && (ghosts[i]->status == FREEDOM || ghosts[i]->status == FREEZE) && RecAreaOverlap(getDrawArea(pman->objData, GAME_TICK_CD), getDrawArea(ghosts[i]->objData, GAME_TICK_CD))) {
			game_log("collide with ghost\n");
			stop_bgm(PACMAN_POWER_UP_SOUND_ID);
			al_rest(1.0);
			pacman_die();
			game_over = true;
			break;
		}
		else if (ghosts[i]->status == FLEE && RecAreaOverlap(getDrawArea(pman->objData, GAME_TICK_CD), getDrawArea(ghosts[i]->objData, GAME_TICK_CD))) {
			ghost_die(ghosts[i]);
			basic_map->score += 100;
		}
	}
}

static void update(void) {

	if (game_over) {
		if (!al_get_timer_started(pman->death_anim_counter)) {          // start death_anim_counter
			al_stop_timer(freeze_timer);
			al_set_timer_count(pman->death_anim_counter, 0);
			al_start_timer(pman->death_anim_counter);
		}
		else if (al_get_timer_count(pman->death_anim_counter) > 70) {   // Pacman's death animation finished and change scene to menu
			al_stop_timer(pman->death_anim_counter);
			al_rest(2.0);
			game_change_scene(scene_menu_create());
		}
		return;
	}

	if (basic_map->beansNum == basic_map->beansCount /* basic_map->beansCount > 5 */) {
		stop_bgm(PACMAN_POWER_UP_SOUND_ID);
		al_rest(1.0);
		game_change_scene(scene_win_create());
		return;
	}

	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++)
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
}

static void draw(void) {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_text(
		regularFont,
		al_map_rgb(255, 255, 255),
		30, 25,
		ALLEGRO_ALIGN_LEFT,
		"Score : "
	);
	al_draw_textf(
		regularFont,
		al_map_rgb(255, 255, 255),
		120, 25,
		ALLEGRO_ALIGN_LEFT,
		"%d",
		basic_map->score
	);

	draw_map(basic_map);

	pacman_draw(pman);

	drawProps(Freeze_item);
	if (!Freeze_item.isUse) {
		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			722, 755,
			ALLEGRO_ALIGN_CENTER,
			"K"
		);
	}

	drawProps(SpeedUp_item);
	if (!SpeedUp_item.isUse) {
		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			671, 755,
			ALLEGRO_ALIGN_CENTER,
			"J"
		);
	}

	if (game_over)
		return;
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);

	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}

}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea(pman->objData, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {
	// free map array, Pacman and ghosts
	delete_map(basic_map);
	pacman_destroy(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_destroy(ghosts[i]);
	}
	al_destroy_timer(power_up_timer);
	al_destroy_timer(freeze_timer);
	al_destroy_timer(speed_up_timer);

	al_destroy_bitmap(Freeze_item.img);
	al_destroy_bitmap(SpeedUp_item.img);
}

static void on_key_down(int key_code) {
	switch (key_code)
	{
	case ALLEGRO_KEY_UP:
	case ALLEGRO_KEY_W:
		pacman_NextMove(pman, UP);
		break;
	case ALLEGRO_KEY_LEFT:
	case ALLEGRO_KEY_A:
		pacman_NextMove(pman, LEFT);
		break;
	case ALLEGRO_KEY_DOWN:
	case ALLEGRO_KEY_S:
		pacman_NextMove(pman, DOWN);
		break;
	case ALLEGRO_KEY_RIGHT:
	case ALLEGRO_KEY_D:
		pacman_NextMove(pman, RIGHT);
		break;
	case ALLEGRO_KEY_C:
		cheat_mode = !cheat_mode;
		if (cheat_mode)
			printf("cheat mode on\n");
		else
			printf("cheat mode off\n");
		break;
	case ALLEGRO_KEY_J:
		if (!SpeedUp_item.isUse) {
			SpeedUp_item.isUse = true;
			al_set_timer_count(speed_up_timer, 0);
			al_start_timer(speed_up_timer);
			pman->speed = SpeedUp_speed;
		}
		break;
	case ALLEGRO_KEY_K:
		if (!Freeze_item.isUse && !pman->powerUp) {
			Freeze_item.isUse = true;
			al_set_timer_count(freeze_timer, 0);
			al_start_timer(freeze_timer);
			for (int i = 0; i < GHOST_NUM; i++) {
				ghost_toggle_FREEZE(ghosts[i], true);
			}
		}
	default:
		break;
	}
}

static void on_mouse_down(void) {
	// nothing here

}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		410, 190,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);

	al_flip_display();
	al_rest(2.0);

}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}
