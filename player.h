#ifndef JAMBOPLAYER
#define JAMBOPLAYER

#include "glob.h"

#define PLAYER_SIZE 24
#define PLAYER_SPEED 3.5
#define PLAYER_SPAWNX 30
#define PLAYER_SPAWNY 30
#define PLAYER_RUN_MOD 1.5
#define PLAYER_JUMP_SPEED -10
#define PLAYER_SOFT_JUMP_SPEED -6
#define PLAYER_SOFT_JUMP_THRES 9


typedef struct Player{
	SDL_Surface* sprite; //NULL
	float spawnX;
	float posX;
	float velX;
	float targVelX;
	float velChangeX;
	float gravX;
	float targCountX;
	float jumpChangeX;

	float spawnY;
	float posY;
	float velY;
	float gravY;
	float targVelY;
	float velChangeY;
	float targCountY;
	
	float speed;
	bool up;
	bool down;
	bool left;
	bool right;
	bool lastUp;
	bool lastLeft;
	bool lastRight;
	bool run;
	bool canJump;
	bool inAir;
	int timeAlive;
	int size;


} Player;



bool player_initplayer(Player *player);
bool player_draw(Player *player, SDL_Surface *surface, float offsetPosX, float offsetPosY);
bool player_input(Player *player);
void player_die(Player *player);
bool player_step(Player *player, SDL_Rect *map);
float player_posX(Player *player);
float player_posY(Player *player);


#endif
