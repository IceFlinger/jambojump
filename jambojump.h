#ifndef JAMBOJUMP
#define JAMBOJUMP



//const float SCREEN_FPS = 60;
//const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

#define ENABLE_GRID true

#define SOLID_COUNT 524288

#define SCROLL_BOUND_L 175
#define SCROLL_BOUND_R 175
#define SCROLL_BOUND_U 250
#define SCROLL_BOUND_D 125

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define SCREEN_FPS 60

typedef struct Console{
	char buffer[1024];
	SDL_Surface *surface;
	SDL_Texture *texture;
	TTF_Font *font;
} Console;

//typedef struct Solid{
//	float x;
//	float y;
//	float w;
//	float h;
//} Solid;
//same struct as SDL_Rect, if more info needed for 
//solids add SDL_Rect member here + other attributes




#endif
