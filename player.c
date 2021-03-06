//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "player.h"


//move these to struct members and access them during draw phase
char const player_color[] = {0x00,0x00,0xc0};
char const player_border[] = {0x4d,0x4d,0xff};

bool player_initsprite(Player *player){
	player->sprite = SDL_CreateRGBSurface(0,player->size,player->size,32,0, 0,0,0);
	SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(player->sprite);
	SDL_FillRect(player->sprite, NULL, SDL_MapRGB(player->sprite->format, player_color[0], player_color[1], player_color[2]));
	SDL_SetRenderDrawColor( renderer, player_border[0], player_border[1], player_border[2] , 0xFF); 
	SDL_RenderDrawRect( renderer, NULL );
}

bool player_initplayer(Player *player){
	bool success = true;
	Player newplayer = {NULL, //sprite
		PLAYER_SPAWNX,	//spawnX
		PLAYER_SPAWNX,	//posX
		0,				//velX
		0,				//targVelX
		0,				//velchangeX
		0,				//gravX
		0,				//targCountX
		0,				//jumpChangeX

		PLAYER_SPAWNY,	//spawnY
		PLAYER_SPAWNY,	//posY
		0,				//velY
		0,				//targVelY

		PLAYER_SPEED,	//speed
		false,			//jump
		false,			//up
		false,			//down
		false,			//left
		false,			//right
		false,			//lastUp
		false,			//lastLeft
		false,			//lastRight
		false,			//run
		false,			//canJump
		false,			//bhop
		false,			//inAir
		false,			//onFloor
		false,			//onWall
		0,				//timeAlive
		PLAYER_SIZE 	//size
	};
	*player = newplayer;
	player_initsprite(player);
	if( player->sprite == NULL )
	{
		printf( "Unable to load player sprite! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	return success;
};

bool player_draw(Player *player, SDL_Surface *surface, float offsetPosX, float offsetPosY){
	//printf("ScrollPos:%f;%f\n", offsetPosX,offsetPosY );
	float drawPosX = (player->posX - offsetPosX);
	float drawPosY = (player->posY - offsetPosY);
	SDL_Rect playerRect = {drawPosX, drawPosY, player->size, player->size};
	//printf("DrawPos:%f;%f\n", drawPosX,drawPosY );
	SDL_BlitSurface( player->sprite, NULL, surface, &playerRect );
};

bool player_input(Player *player){
	const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
	player->jump = currentKeyStates[ SDL_SCANCODE_X ];
	player->up = currentKeyStates[ SDL_SCANCODE_UP ];
	player->down = currentKeyStates[ SDL_SCANCODE_DOWN ];
	player->left = currentKeyStates[ SDL_SCANCODE_LEFT ];
	player->right = currentKeyStates[ SDL_SCANCODE_RIGHT ];
	player->run = currentKeyStates[ SDL_SCANCODE_Z ];
};
bool player_step(Player *player, SDL_Rect *map, int map_size){
	if(player->run){
		player->speed = PLAYER_SPEED * PLAYER_RUN_MOD;
	} else {
		player->speed = PLAYER_SPEED;
	}
	player->timeAlive++;

	player->targVelX = player->gravX + player->jumpChangeX*player->inAir;

	if(player->jump && player->canJump){
		if ((player->onWall)&&!(player->up||player->right||player->left)){
			player->velY = PLAYER_SOFT_JUMP_SPEED;
		} else {
			player->velY = PLAYER_JUMP_SPEED;
		}
		if (player->jumpChangeX != 0.0){
			player->velX = player->jumpChangeX;
		}
		player->canJump = false;
	}

	if(player->left && player->right){
		if(player->left && !(player->lastLeft)){
			player->targVelX = -player->speed;
			player->targCountX = 0.0;
		} else if (player->right && !(player->lastRight)){
			player->targVelX = player->speed;
			player->targCountX = 0.0;
		}
	} else if (player->left){
		player->targVelX = -player->speed;
		if(!(player->lastLeft)){
			player->targCountX = 0.0;
		}
	} else if (player->right){
		player->targVelX = player->speed;
		if(!(player->lastRight)){
			player->targCountX = 0.0;
		}
	}

	if(!(player->jump) && player->lastJump && (player->velY < 0.0) && (player->jumpChangeX == 0.0)){
		//Reduce our upwards speed if we release the button while rising
		player->velY = PLAYER_EARLY_DROP_SPEED; 
	}
	if(!(player->left)&&!(player->right)&&!(player->inAir)){
		//quickly stop if we're on the ground and not inputting a direction
		player->targCountX = 0.0;
	}

	player->lastJump = player->jump;
	player->lastLeft = player->left;
	player->lastRight = player->right;

	
	if(player->velY != player->targVelY){
		float oldVelY = player->velY;
		if(player->velY < player->targVelY){
			player->velY += PLAYER_VETICAL_FRICTION;
			if(player->velY > player->targVelY){
				player->velY = player->targVelY;
			}
		} else {
			player->velY -= PLAYER_VETICAL_FRICTION;
			if(player->velY < player->targVelY){
				player->velY = player->targVelY;
			}
		}
	}

	//cancel air momentum if pushing opposite way
	/*if(((((player->targVelX)*(player->velX)) < 0)&&player->inAir)&&(abs(player->velX)<abs(player->jumpChangeX-player->velChangeX))){
		player->jumpChangeX = 0;
	}*/
	if(fabs(player->velX)<fabs(player->jumpChangeX)){
		if (player->jumpChangeX<0){
			player->jumpChangeX = -fabs(player->velX);
		} else {
			player->jumpChangeX = fabs(player->velX);
		}
	}
	
	if (player->velX != player->targVelX){
		if(player->targCountX == 0.0)
		{
			player->targCountX = PLAYER_HORIZONTAL_TURN_FRICTION;
			if (player->targVelX == 0.0){
				player->targCountX = PLAYER_HORIZONTAL_FRICTION;
			}
		} else {
			player->targCountX *= player->velChangeX;
		}
		float oldVelX = player->velX;
		if (player->velX < player->targVelX){
			player->velX += player->targCountX;
			if(player->velX > player->targVelX){
				player->velX = player->targVelX;
				player->targCountX = 0.0;
			}
		} else {
			player->velX -= player->targCountX;
			if(player->velX < player->targVelX){
				player->velX = player->targVelX;
				player->targCountX = 0.0;
			}
		}
		if(oldVelX * player->velX < 0.0){
			player->targCountX = 0.0;
		}
	}



	bool collided = false;
	float oldX = player->posX;
	float oldY = player->posY;
	player->posY += player->velY;
	player->posX += player->velX;
	//printf("Checking solid collision\n");
	for(int i = 0; i < map_size; i++){
		//(playeredge)(solidedge)(playeredge)
		//printf("Checking solid %d\n", i);
		if (map[i].w != 0){
			if(!((player->posX > map[i].x+map[i].w)||(player->posX+player->size < map[i].x))){
				if(!((player->posY > map[i].y+map[i].h)||(player->posY+player->size < map[i].y))){
					//top edge
					if((oldY+player->size)<=map[i].y){
						//printf("top\n");
						player->posY = map[i].y - player->size;
						player->velY = 0.0;
						collided = true;
						player->onFloor = true;
						player->onWall = false;
						player->jumpChangeX = 0.0;
					}
					//bottom edge
					else if(oldY>=(map[i].y + map[i].h)){
						//printf("bottom\n");
						player->posY = map[i].y + map[i].h;
						player->velY = 0.0;
					}
					//left edge
					else if((oldX+player->size)<=map[i].x){
						//printf("left\n");
						player->posX = map[i].x - player->size;
						player->gravX = PLAYER_WALLJUMP_MOMENTUM;
						player->targCountX = 0.0;
						if (!collided){
							player->jumpChangeX = -PLAYER_WALLJUMP_XSPEED;
							collided = true;
							player->onWall = true;
						}
						player->velX = PLAYER_WALL_STICKINESS; //stick towards the wall
					}
					//right edge
					else if(oldX>=(map[i].x + map[i].w)){
						//printf("right\n");
						player->posX = map[i].x + map[i].w;
						player->gravX = -PLAYER_WALLJUMP_MOMENTUM;
						player->targCountX = 0.0;
						if(!collided){
							player->jumpChangeX = PLAYER_WALLJUMP_XSPEED;
							collided = true;
							player->onWall = true;
						}
						player->velX = -PLAYER_WALL_STICKINESS; //works anywhere between 0.01 and 1.9 but not more?
					}
				}
			}
		}
	}
	if(collided){
		if(!player->lastJump){
			//avoid bouncing
			player->canJump = true;
		} else if (player->bhop) {
			//allow one bounce if we previously flagged a bhop
			player->canJump = true;
		} else {
			player->canJump = false;
		}
		player->bhop = false;
		if (player->jumpChangeX == 0.0){
			player->gravX = 0.0;
		}
		player->velChangeX = 2.0;
		player->inAir = false;
		if(player->up||player->right||player->left){
			player->targVelY = PLAYER_WALL_GRAV;
		} else{
			player->targVelY = PLAYER_AIR_GRAV;
		}
	} else {
		player->canJump = false;
		//not holding jump and falling, enable bhop next landing
		//if(!(player->jump)&&(player->velY > 0)){
		if((!(player->jump))&&(player->velY > 0)){
			player->bhop = true;
		}
		player->gravX = 0.0;
		player->velChangeX = 1.0;
		player->inAir = true;
		player->onFloor = false;
		player->onWall = false;
		player->targVelY = PLAYER_AIR_GRAV;
	}
	//printf("Checked collision\n");

};

void player_die(Player *player){
	player->posX = player->spawnX;
	player->posY = player->spawnY;
	player->velX = 0.0;
	player->velY = 0.0;
	player->timeAlive = 0;
}

void player_debug(Player *player, char *buffer){
	if(PLAYER_DEBUG){
		sprintf(buffer + strlen(buffer), "spawnX: %f\n", player->spawnX );
		sprintf(buffer + strlen(buffer), "posX: %f\n", player->posX );
		sprintf(buffer + strlen(buffer), "velX: %f\n", player->velX );
		sprintf(buffer + strlen(buffer), "targVelX: %f\n", player->targVelX );
		sprintf(buffer + strlen(buffer), "velChangeX: %f\n", player->velChangeX );
		sprintf(buffer + strlen(buffer), "gravX: %f\n", player->gravX );
		sprintf(buffer + strlen(buffer), "targCountX: %f\n", player->targCountX );
		sprintf(buffer + strlen(buffer), "jumpChangeX: %f\n", player->jumpChangeX );

		sprintf(buffer + strlen(buffer), "spawnY: %f\n", player->spawnY );
		sprintf(buffer + strlen(buffer), "posY: %f\n", player->posY );
		sprintf(buffer + strlen(buffer), "velY: %f\n", player->velY );
		sprintf(buffer + strlen(buffer), "targVelY: %f\n", player->targVelY );

		sprintf(buffer + strlen(buffer), "speed: %f\n", player->speed );
		sprintf(buffer + strlen(buffer), "jump: %d\n", player->jump );
		sprintf(buffer + strlen(buffer), "up: %d\n", player->up );
		sprintf(buffer + strlen(buffer), "down: %d\n", player->down );
		sprintf(buffer + strlen(buffer), "left: %d\n", player->left );
		sprintf(buffer + strlen(buffer), "right: %d\n", player->right );
		sprintf(buffer + strlen(buffer), "lastJump: %d\n", player->lastJump );
		sprintf(buffer + strlen(buffer), "lastLeft: %d\n", player->lastLeft );
		sprintf(buffer + strlen(buffer), "lastRight: %d\n", player->lastRight );
		sprintf(buffer + strlen(buffer), "run: %d\n", player->run );
		sprintf(buffer + strlen(buffer), "canJump: %d\n", player->canJump );
		sprintf(buffer + strlen(buffer), "bhop: %d\n", player->bhop );
		sprintf(buffer + strlen(buffer), "inAir: %d\n", player->inAir );
		sprintf(buffer + strlen(buffer), "onFloor: %d\n", player->onFloor );
		sprintf(buffer + strlen(buffer), "onWall: %d\n", player->onWall );
		sprintf(buffer + strlen(buffer), "timeAlive: %d\n", player->timeAlive );
		sprintf(buffer + strlen(buffer), "size: %d\n", player->size );
	}
}

