#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_ttf.h>
#include "fontData.h"
#include "jambojump.h"
#include "player.h"

// Real global constants i guess

int used_solids = 0;
char const background_color[] = {0xd9,0xd9,0xd9};
char const grid_color[] = {0xd0,0xd0,0xd0};
char const solid_color[] = {0x00,0x00,0x00};
SDL_Color conColor = { 0, 0, 0 };

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


SDL_Rect map[SOLID_COUNT];

struct Player *player;
struct Console *con;

//end constants

bool init(){
	bool success = false;
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		window = SDL_CreateWindow( "Jambojump", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else
		{
			if(TTF_Init()==-1) {
			    printf( "SDL_TTF could not initialize! TTF_Error: %s\n", TTF_GetError() );
			} else {
				surface = SDL_GetWindowSurface( window );
				success = true;
			}
		}
	}
	return success;
};

void add_solid(int x, int y, int w, int h){
	if(used_solids<SOLID_COUNT){
		map[used_solids].x = x;
		map[used_solids].y = y;
		map[used_solids].w = w;
		map[used_solids].h = h;
		used_solids++;
	}
}

void console_loadfont(Console *console, TTF_Font *font){
	console->font = font;
}

bool load(){
	//Loading success 
	bool success = true;
	success = player_initplayer(&player);
	//printf("Initializing console\n");
	//Console *newconsole = {{0}, NULL, NULL, NULL};
	//con = newconsole;
	//printf("Console initialized, loading font from mem\n");
	//console_loadfont(&con, TTF_OpenFontRW(SDL_RWFromConstMem(fontdata, FONT_DATA_LEN), 1, 9));
	//con->font =  TTF_OpenFontRW(SDL_RWFromConstMem(fontdata, FONT_DATA_LEN), 1, 9);
	//printf("Font loaded\n");
	//init blocks
	for(int i = 0; i < SOLID_COUNT; i++){
		map[i].x = 0;
		map[i].y = 0;
		map[i].w = 0;
		map[i].h = 0;
	}
//	add_solid(SCREEN_WIDTH/4, 500, SCREEN_WIDTH/2, 30);
	add_solid(-800, 0.0, 810, 600); //lay out some basic shapes
	add_solid(900, 0.0, 800, 600);
	add_solid(10, 590, 400, 10);
	add_solid(700, 590, 200, 10);
	add_solid(700, 600, 10, 500);
	add_solid(400, 600, 10, 500);
	add_solid(400, 1090, 310, 10);
	add_solid(10, 0.0, 800, 10);
	add_solid(10, 500, 100, 10);
	add_solid(40, 450, 100, 10);
	add_solid(270, 440, 30, 10);
	add_solid(100, 350, 100, 10);
	add_solid(300, 300, 500, 200);
	add_solid(300, 10, 500, 200);

	return success;
};

void draw(){
	SDL_FillRect( surface, NULL, SDL_MapRGB( surface->format, background_color[0], background_color[1], background_color[2]) );
	SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
	if(ENABLE_GRID){
		int horzLines = ceil(SCREEN_WIDTH/PLAYER_SIZE);
		int vertLines = ceil(SCREEN_HEIGHT/PLAYER_SIZE);
		SDL_SetRenderDrawColor( renderer, grid_color[0], grid_color[1], grid_color[2] , 0xFF); 
		for (int i = 0; i <= horzLines; i++){
			float x1 = PLAYER_SIZE*i - (int)scrollPosX%PLAYER_SIZE;
			float y1 = 0;
			float x2 = PLAYER_SIZE*i - (int)scrollPosX%PLAYER_SIZE;
			float y2 = SCREEN_HEIGHT - (int)scrollPosY%PLAYER_SIZE;
			SDL_RenderDrawLine( renderer, x1, y1, x2, y2);
		}
		for (int i = 0; i <= vertLines; i++){
			float x1 = 0;
			float y1 = PLAYER_SIZE*i- (int)scrollPosY%PLAYER_SIZE;
			float x2 = SCREEN_WIDTH- (int)scrollPosX%PLAYER_SIZE;
			float y2 = PLAYER_SIZE*i- (int)scrollPosY%PLAYER_SIZE;
			SDL_RenderDrawLine( renderer, x1, y1, x2, y2);
		}
		for(int i = 0; i < SOLID_COUNT; i++){
			if(map[i].w != 0){
				SDL_Rect solidRect = {map[i].x - scrollPosX, map[i].y - scrollPosY, map[i].w, map[i].h};
				SDL_FillRect( surface, &solidRect, SDL_MapRGB( surface->format, solid_color[0],solid_color[1], solid_color[2]) );
			}
		}
	}
	player_draw(&player, surface, scrollPosX, scrollPosY);

	//con->surface = TTF_RenderUTF8_Solid( con->font, con->buffer, conColor );
	//con->texture = SDL_CreateTextureFromSurface( renderer, con->surface );
	//SDL_RenderCopy(renderer, con->texture, NULL, &con->surface->clip_rect);
	//memset(con->buffer, 0, 1024);
	SDL_UpdateWindowSurface( window );
};

void step(){
	SDL_Event e;
	//strncpy(&con->buffer, "test string123123", 11);
	//printf("Polling input\n");
	while( SDL_PollEvent( &e ) != 0 ){
		//User requests quit
		if( e.type == SDL_QUIT ){
			running = false;
		}
		else if( e.type == SDL_KEYDOWN )
		{
			//Poll for keykodes
			switch( e.key.keysym.sym )
			{
				case SDLK_ESCAPE:
				player_die(&player);
				break;
				default:
				break;
			}
		}
		//printf("Event processed\n");
	}
	//printf("Passing input to player\n");	
	player_input(&player);
	//printf("Beginning player step\n");
	player_step(&player, &map, SOLID_COUNT);
	//printf("Player step complete\n");
	//move the camera
	float centerX = (scrollPosX+(SCREEN_WIDTH/2));
	float centerY = (scrollPosY+(SCREEN_HEIGHT/2));
	if(player_posX(&player) + scrollBoundL < centerX){
		scrollPosX -= centerX - (player_posX(&player) + scrollBoundL);
	}
	if(player_posX(&player) - scrollBoundR > centerX){
		scrollPosX += (player_posX(&player) - scrollBoundR) - centerX;
	}
	if(player_posY(&player) + scrollBoundU < centerY){
		scrollPosY -= centerY - (player_posY(&player) + scrollBoundU);
	}
	if(player_posY(&player) - scrollBoundD > centerY){
		scrollPosY += (player_posY(&player) - scrollBoundD) - centerY;
	}
};

void quit(){
	//Destroy window
	SDL_DestroyWindow( window );
	TTF_Quit();
	//Quit SDL subsystems
	SDL_Quit();
};

int main( int argc, char* args[] )
{
	printf("Starting...\n");
	printf("SOLID_COUNT: %d\n", SOLID_COUNT);
	if ( !init() ){
		printf("Failed to initialize\n");
		return 1;
	}
	printf("Initialized...\n");
	if ( !load() ){
		printf("Failed to load image data\n");
		return 1;
	}
	printf("Loaded %d/%d solids\n", used_solids, SOLID_COUNT);
	printf("Loaded and running...\n");
	running = true;
	int countedFrames = 0;
	while(running){
		float tickstart = SDL_GetTicks();
		//printf( "Entering tick loop\n");
		step();
		//printf( "Done tick, drawing\n");
		draw();
		//player_debug(&player);
		//printf( "Done drawing, continuing\n");
		float tickend = SDL_GetTicks();
		float frameticks = tickend - tickstart;
		if( frameticks < (1000/SCREEN_FPS) )
			{
				float timeout =SDL_GetTicks() + ((1000/SCREEN_FPS)-frameticks);
				while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {
					
				}
			}
		//Calculate and correct fps
		float avgFPS = countedFrames / ( tickstart / 1000.0 );
		if( avgFPS > 2000000 ){
			avgFPS = 0;
		} //need some kind of in-window console output for debuggings
		//printf("%d/%f=%f:%f;%f\r", countedFrames,tickstart,avgFPS,scrollPosX,scrollPosY);
		countedFrames++;
	}
	printf("\nExiting...");
	quit();
	return 0;
}
