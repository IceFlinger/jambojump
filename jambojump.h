#ifndef JAMBOJUMP
#define JAMBOJUMP

#include "glob.h"
#include "player.h"

//const float SCREEN_FPS = 60;
//const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

#define ENABLE_GRID true

int used_solids = 0;

char const background_color[] = {0xd9,0xd9,0xd9};
char const grid_color[] = {0xd0,0xd0,0xd0};
char const solid_color[] = {0x00,0x00,0x00};

bool running = false;

float scrollPosX = 0;
float scrollPosY = 0;
float scrollBoundL = SCROLL_BOUND_L;
float scrollBoundR = SCROLL_BOUND_R;
float scrollBoundU = SCROLL_BOUND_U;
float scrollBoundD = SCROLL_BOUND_D;


//The window we'll be rendering to
SDL_Window* window = NULL;

//The surface contained by the window
SDL_Surface* surface = NULL;

//typedef struct Solid{
//	float x;
//	float y;
//	float w;
//	float h;
//} Solid;
//same struct as SDL_Rect, if more info needed for 
//solids add SDL_Rect member here + other attributes


SDL_Rect map[SOLID_COUNT];

struct Player *player;

#endif
