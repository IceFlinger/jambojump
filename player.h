#ifndef JAMBOPLAYER
#define JAMBOPLAYER

#include "glob.h"

#define PLAYER_SIZE 24
#define PLAYER_SPEED 4
#define PLAYER_SPAWNX 30
#define PLAYER_SPAWNY 30
#define PLAYER_RUN_MOD 1.4
#define PLAYER_JUMP_SPEED -11 //-10
#define PLAYER_AIR_GRAV 14 //10
#define PLAYER_WALL_GRAV 8 //7
#define PLAYER_WALL_STICKINESS 0.01 //betwee 0 and 0.2
#define PLAYER_WALLJUMP_XSPEED 5.0 //horizontal mod for jumping off wall
#define PLAYER_WALLJUMP_MOMENTUM 1.0 //momentum after walljumping
#define PLAYER_SOFT_JUMP_SPEED -7 //-6
#define PLAYER_SOFT_JUMP_THRES 10 //9, between air grav and wall grav
#define PLAYER_EARLY_DROP_SPEED -2 //speed to go when we release jump button
#define PLAYER_VETICAL_FRICTION 0.5 //Rate at which we reach our target vertical velocity
#define PLAYER_HORIZONTAL_FRICTION 0.6
#define PLAYER_HORIZONTAL_TURN_FRICTION 0.2
#define PLAYER_DEBUG false


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
	bool bhop;
	bool inAir;
	int timeAlive;
	int size;


} Player;



bool player_initplayer(Player *player);
bool player_draw(Player *player, SDL_Surface *surface, float offsetPosX, float offsetPosY);
bool player_input(Player *player);
void player_die(Player *player);
void player_debug(Player *player);
bool player_step(Player *player, SDL_Rect *map);
float player_posX(Player *player);
float player_posY(Player *player);


#endif
