#include "shared.h"
#include "utility.h"
#include "game.h"

ALLEGRO_SAMPLE* themeMusic = NULL;
ALLEGRO_SAMPLE* PACMAN_MOVESOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_POWER_UP_SOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_WIN_SOUND = NULL;
ALLEGRO_FONT* menuFont = NULL;
ALLEGRO_FONT* regularFont = NULL;
int menuFontSize = 30;
int regularFontSize = 25;
float music_volume = 0.5;
float effect_volume = 0.5;
bool gameDone = false;

/*
	A way to accelerate is load assets once.
	And delete them at the end.
	This method does provide you better management of memory.
*/
void shared_init(void) {
	
	menuFont = load_font("Assets/Minecraft.ttf", menuFontSize);
	regularFont = load_font("Assets/OpenSans-Regular.ttf", regularFontSize);
	themeMusic = load_audio("Assets/Music/original_theme.ogg");
	PACMAN_MOVESOUND = load_audio("Assets/Music/pacman-chomp.ogg");
	PACMAN_DEATH_SOUND = load_audio("Assets/Music/pacman_death.ogg");
	PACMAN_POWER_UP_SOUND = load_audio("Assets/Music/pacman_power_up.mp3");
	PACMAN_WIN_SOUND = load_audio("Assets/Music/pacman_win.mp3");
}

void shared_destroy(void) {

	al_destroy_font(menuFont);
	al_destroy_font(regularFont);
	al_destroy_sample(themeMusic);
	al_destroy_sample(PACMAN_MOVESOUND);
	al_destroy_sample(PACMAN_DEATH_SOUND);
	al_destroy_sample(PACMAN_POWER_UP_SOUND);
	al_destroy_sample(PACMAN_WIN_SOUND);
}