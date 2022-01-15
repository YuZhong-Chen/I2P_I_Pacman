#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include "ghost.h"
#include "map.h"
#include "pacman_obj.h"

/* global variables*/
// [ NOTE ]
// if you change the map .txt to your own design.
// You have to modify cage_grid_{x,y} to corressponding value also.
// Or you can do some change while loading map (reading .txt file)
// Make the start position metadata stored with map.txt.
const int cage_grid_x = 30, cage_grid_y = 11;

/* shared variables. */
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern const int block_width, block_height;
extern ALLEGRO_TIMER* power_up_timer;
extern const int power_up_duration;

/* Internal variables */
static const int fix_draw_pixel_offset_x = -3;
static const int fix_draw_pixel_offset_y = -3;
static const int draw_region = 30;
static const int animate_mask = 1 << 5;
static int animate_state = 0;
// [ NOTE - speed again ]
// Again, you see this notaficationd. If you still want to implement something 
// fancy with speed, objData->moveCD and GAME_TICK, you can first start on 
// working on animation of ghosts and pacman. // Once you finished the animation 
// part, you will have more understanding on whole mechanism.
static const int basic_speed = 2;

Ghost* ghost_create(int flag) {

	// NOTODO
	Ghost* ghost = (Ghost*)malloc(sizeof(Ghost));
	if (!ghost)
		return NULL;

	ghost->typeFlag = flag;
	ghost->objData.Size.x = block_width;
	ghost->objData.Size.y = block_height;

	ghost->objData.nextTryMove = NONE;
	ghost->speed = basic_speed;
	ghost->status = BLOCKED;

	ghost->flee_sprite = load_bitmap("Assets/ghost_flee.png");
	ghost->dead_sprite = load_bitmap("Assets/ghost_dead.png");

	switch (ghost->typeFlag) {
	case Blinky:
		ghost->objData.Coord.x = cage_grid_x;
		ghost->objData.Coord.y = cage_grid_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
		ghost->move_script = &ghost_red_move_script;
		break;
	case Pinky:
		ghost->objData.Coord.x = cage_grid_x;
		ghost->objData.Coord.y = cage_grid_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_pink.png");
		ghost->move_script = &ghost_pink_move_script;
		break;
	case Inky:
		ghost->objData.Coord.x = cage_grid_x;
		ghost->objData.Coord.y = cage_grid_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_blue.png");
		ghost->move_script = &ghost_blue_move_script;
		break;
	case Clyde:
		ghost->objData.Coord.x = cage_grid_x;
		ghost->objData.Coord.y = cage_grid_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_orange.png");
		ghost->move_script = &ghost_orange_move_script;
		break;
	default:
		ghost->objData.Coord.x = cage_grid_x;
		ghost->objData.Coord.y = cage_grid_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
		ghost->move_script = &ghost_red_move_script;
		break;
	}
	return ghost;
}
void ghost_destroy(Ghost* ghost) {
	// free ghost resource
	al_destroy_bitmap(ghost->move_sprite);
	al_destroy_bitmap(ghost->flee_sprite);
	al_destroy_bitmap(ghost->dead_sprite);
	free(ghost);
}
void ghost_draw(Ghost* ghost) {
	RecArea drawArea = getDrawArea(ghost->objData, GAME_TICK_CD);

	if (ghost->status == FLEE) {
		animate_state = (ghost->objData.moveCD & animate_mask) > 0 ? 16 : 0;
		if (al_get_timer_count(power_up_timer) > 0.7 * power_up_duration && (ghost->objData.moveCD & animate_mask)) {
			al_draw_scaled_bitmap(ghost->flee_sprite, 32 + animate_state, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
		}
		else {
			al_draw_scaled_bitmap(ghost->flee_sprite, 0 + animate_state, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
		}
	}
	else if (ghost->status == GO_IN) {
		switch (ghost->objData.facing)
		{
		case RIGHT:
			al_draw_scaled_bitmap(ghost->dead_sprite, 0, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case LEFT:
			al_draw_scaled_bitmap(ghost->dead_sprite, 16, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case UP:
			al_draw_scaled_bitmap(ghost->dead_sprite, 32, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case DOWN:
			al_draw_scaled_bitmap(ghost->dead_sprite, 48, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		default:
			al_draw_scaled_bitmap(ghost->dead_sprite, 0, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		}
	}
	else {
		animate_state = (ghost->objData.moveCD & animate_mask) > 0 ? 16 : 0;
		switch (ghost->objData.facing)
		{
		case RIGHT:
			al_draw_scaled_bitmap(ghost->move_sprite, 0 + animate_state, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case LEFT:
			al_draw_scaled_bitmap(ghost->move_sprite, 32 + animate_state, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case UP:
			al_draw_scaled_bitmap(ghost->move_sprite, 64 + animate_state, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case DOWN:
			al_draw_scaled_bitmap(ghost->move_sprite, 96 + animate_state, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		default:
			al_draw_scaled_bitmap(ghost->move_sprite, 0 + animate_state, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		}
	}
}
void ghost_NextMove(Ghost* ghost, Directions next) {
	ghost->objData.nextTryMove = next;
}
void printGhostStatus(GhostStatus S) {

	switch (S) {

	case BLOCKED: // stay inside the ghost room
		game_log("BLOCKED");
		break;
	case GO_OUT: // going out the ghost room
		game_log("GO_OUT");
		break;
	case FREEDOM: // free at the map
		game_log("FREEDOM");
		break;
	case GO_IN:
		game_log("GO_IN");
		break;
	case FLEE:
		game_log("FLEE");
		break;
	default:
		game_log("status error");
		break;
	}
}
bool ghost_movable(Ghost* ghost, Map* M, Directions targetDirec, bool room) {
	// Determine if the current direction is movable.
	int Grid_x = ghost->objData.Coord.x, Grid_y = ghost->objData.Coord.y;

	switch (targetDirec)
	{
	case UP:
		return !(is_wall_block(M, Grid_x, Grid_y - 1) || (room && is_room_block(M, Grid_x, Grid_y - 1)));
	case DOWN:
		return !(is_wall_block(M, Grid_x, Grid_y + 1) || (room && is_room_block(M, Grid_x, Grid_y + 1)));
	case LEFT:
		return !(is_wall_block(M, Grid_x - 1, Grid_y) || (room && is_room_block(M, Grid_x - 1, Grid_y)));
	case RIGHT:
		return !(is_wall_block(M, Grid_x + 1, Grid_y) || (room && is_room_block(M, Grid_x + 1, Grid_y)));
	default:
		// for none UP, DOWN, LEFT, RIGHT direction u should return false.
		return false;
	}

	return true;
}

void ghost_toggle_FLEE(Ghost* ghost, bool setFLEE) {
	// Here is reserved for power bean implementation.
	// The concept is "When pacman eats the power bean, only
	// ghosts who are in state FREEDOM will change to state FLEE.
	// For those who are not (BLOCK, GO_IN, etc.), they won't change state."
	// This implementation is based on the classic PACMAN game.
	if (setFLEE) {
		if (ghost->status == FREEDOM) {
			ghost->status = FLEE;
			ghost->speed = 1;
		}
	}
	else {
		if (ghost->status == FLEE) {
			ghost->status = FREEDOM;
			ghost->speed = basic_speed;
		}
	}
}

void ghost_die(Ghost* ghost) {
	ghost->status = GO_IN;
	ghost->speed = 4;
}

void ghost_move_script_GO_IN(Ghost* ghost, Map* M) {
	ghost_NextMove(ghost, shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, cage_grid_x, cage_grid_y));
}
void ghost_move_script_GO_OUT(Ghost* ghost, Map* M) {
	if (M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B')
		ghost_NextMove(ghost, UP);
	else
		ghost->status = FREEDOM;
}
void ghost_move_script_FLEE(Ghost* ghost, Map* M, const Pacman* const pacman) {
	// The concept here is to simulate ghosts running away from pacman while pacman is having power bean ability.
	Directions shortestDirection = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, pacman->objData.Coord.x, pacman->objData.Coord.y);

	// possible movement
	Directions proba[4];
	int cnt = 0;
	for (Directions i = 1; i <= 4; i++) {
		if (ghost_movable(ghost, M, i, true) && (5 - ghost->objData.preMove) != i && shortestDirection != i) {
			proba[cnt++] = i;
		}
	}

	if (cnt >= 1) {
		ghost_NextMove(ghost, proba[generateRandomNumber(0, cnt - 1)]);
	}
	else {
		cnt = 0;
		for (Directions i = 1; i <= 4; i++) {
			if (ghost_movable(ghost, M, i, true) && (5 - ghost->objData.preMove) != i) {
				proba[cnt++] = i;
			}
		}
		if (cnt >= 1) {
			ghost_NextMove(ghost, proba[generateRandomNumber(0, cnt - 1)]);
		}
		else {
			ghost_NextMove(ghost, 5 - ghost->objData.preMove);
		}
	}
}

