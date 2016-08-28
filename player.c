//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "player.h"


//move these to struct members and access them during draw phase
char const player_color[] = {0x00,0x00,0xc0};
char const player_border[] = {0x4d,0x4d,0xff};

bool player_initplayer(Player *player){
	bool success = true;
	Player newplayer = {NULL, //sprite
		PLAYER_SPAWNX,	//spawnX
		PLAYER_SPAWNX,	//posX
		0,				//velX
		0,				//targVelX
		1.5,			//velchangeX
		5,				//gravX
		0,				//targCountX
		0,				//jumpChangeX

		PLAYER_SPAWNY,	//spawnY
		PLAYER_SPAWNY,	//posY
		0,				//velY
		10.0,			//gravY
		0,				//targVelY
		1.2,			//velchangeY
		0,				//targCountY

		PLAYER_SPEED,	//speed
		false,			//up
		false,			//down
		false,			//left
		false,			//right
		false,			//lastUp
		false,			//lastLeft
		false,			//lastRight
		false,			//run
		false,			//canJump
		false,			//inAir
		0,				//timeAlive
		PLAYER_SIZE 	//size
	};
	*player = newplayer;
	player->sprite = SDL_CreateRGBSurface(0,player->size,player->size,32,0, 0,0,0);
	SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(player->sprite);
	SDL_FillRect(player->sprite, NULL, SDL_MapRGB(player->sprite->format, player_color[0], player_color[1], player_color[2]));
	SDL_SetRenderDrawColor( renderer, player_border[0], player_border[1], player_border[2] , 0xFF); 
	SDL_RenderDrawRect( renderer, NULL );
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
	player->up = currentKeyStates[ SDL_SCANCODE_X ];
	player->down = currentKeyStates[ SDL_SCANCODE_DOWN ];
	player->left = currentKeyStates[ SDL_SCANCODE_LEFT ];
	player->right = currentKeyStates[ SDL_SCANCODE_RIGHT ];
	player->run = currentKeyStates[ SDL_SCANCODE_Z ];
};
bool player_step(Player *player, SDL_Rect *map){
	if(player->run){
		player->speed = PLAYER_SPEED * PLAYER_RUN_MOD;
	} else {
		player->speed = PLAYER_SPEED;
	}
	player->timeAlive++;
	player->targVelY = player->gravY;
	player->targVelX = player->gravX;
	player->targCountY = 0.0;

	if(player->up && player->canJump){
		if (player->velY > PLAYER_SOFT_JUMP_THRES){
			player->velY = PLAYER_SOFT_JUMP_SPEED;
		} else {
			player->velY = PLAYER_JUMP_SPEED;
		}
		if (player->jumpChangeX != 0.0){
			player->velX = player->jumpChangeX;
		}
		player->canJump = false;
		if (!(player->lastUp)){
			player->targCountY = 0.0;
		}
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

	if(!(player->up) && player->lastUp && (player->velY < 0.0) && (player->jumpChangeX == 0.0)){
		//what the fuck
		player->velY = -2.0; //??????
	}
	if(!(player->left)&&!(player->right)&&!(player->inAir)){
		player->targCountX = 0.0;
	}

	player->lastUp = player->up;
	player->lastLeft = player->left;
	player->lastRight = player->right;

	if(player->velY < player->targVelY){
		if (player->targCountY == 0.0){
			player->targCountY = 0.5;
		} else{
			player->targCountY *= player->velChangeY;
		}
		float oldVelY = player->velY;
		player->velY += player->targCountY;
		if (oldVelY * player->velY < 0.0){
			player->targCountY = 0.0;
		}
		if(player->velY > player->targVelY){
			player->velY = player->targVelY;
			player->targCountY = 0.0;
		}
	}
	if(player->velY > player->targVelY){
		if (player->targCountY == 0.0){
			player->targCountY = 0.5;
		} else{
			player->targCountY *= player->velChangeY;
		}
		float oldVelY = player->velY;
		player->velY -= player->targCountY;
		if (oldVelY * player->velY < 0.0){
			player->targCountY = 0.0;
		}
		if(player->velY < player->targVelY){
			player->velY = player->targVelY;
			player->targCountY = 0.0;
		}
	}

	if (player->velX < player->targVelX){
		if(player->targCountX == 0.0)
		{
			player->targCountX = 0.2;
			if (player->targVelX == 0.0){
				player->targCountX = 0.6;
			}
		} else {
			player->targCountX *= player->velChangeX;
		}
		float oldVelX = player->velX;
		player->velX += player->targCountX;
		if(oldVelX * player->velX < 0.0){
			player->targCountX = 0.0;
		}
		if(player->velX > player->targVelX){
			player->velX = player->targVelX;
			player->targCountX = 0.0;
		}
	}
	if (player->velX > player->targVelX){
		if(player->targCountX == 0.0)
		{
			player->targCountX = 0.2;
			if (player->targVelX == 0.0){
				player->targCountX = 0.6;
			}
		} else {
			player->targCountX *= player->velChangeX;
		}
		float oldVelX = player->velX;
		player->velX -= player->targCountX;
		if(oldVelX * player->velX < 0.0){
			player->targCountX = 0.0;
		}
		if(player->velX < player->targVelX){
			player->velX = player->targVelX;
			player->targCountX = 0.0;
		}
	}

	bool collided = false;
	float oldX = player->posX;
	float oldY = player->posY;
	player->posY += player->velY;
	player->posX += player->velX;
	//printf("Checking solid collision\n");
	for(int i = 0; i < SOLID_COUNT; i++){
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
						player->gravX = 1.0;
						player->targCountX = 0.0;
						if (!collided){
							player->jumpChangeX = -5.0;
							collided = true;
						}
						player->velX = 0.01; //stick towards the wall
					}
					//right edge
					else if(oldX>=(map[i].x + map[i].w)){
						//printf("right\n");
						player->posX = map[i].x + map[i].w;
						player->gravX = -1.0;
						player->targCountX = 0.0;
						if(!collided){
							player->jumpChangeX = 5.0;
							collided = true;
						}
						player->velX = -0.01; //works anywhere between 0.01 and 1.9 but not more?
					}
				}
			}
		}
	}
	if(collided){
		if(!player->lastUp){
			player->canJump = true;
		} else {
			player->canJump = false;
		}
		if (player->jumpChangeX == 0.0){
			player->gravX = 0.0;
		}
		player->velChangeX = 2.0;
		player->inAir = false;
	} else {
		player->canJump = false;
		player->gravX = 0.0;
		player->velChangeX = 1.0;
		player->inAir = true;
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

float player_posX(Player *player){
	return player->posX;
};
float player_posY(Player *player){
	return player->posY;
};