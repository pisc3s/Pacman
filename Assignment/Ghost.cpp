//My Code Start
#include "Ghost.h"
#include <stdlib.h>
#include <ctime>

//Constructor
Ghost::Ghost() {
	VEL = 1;
	pacman = NULL;
	filePath = "";
	reset();
}

//Initialize everything and load all the media
//Return false if fails to laod
bool Ghost::init(int priority, int xPos, int yPos, std::vector<std::vector<std::string>> _map, std::string file) {
	PRIORITY = priority;
	x = xPos * 30;
	y = yPos * 30;
	xDefault = x;
	yDefault = y;
	map = _map;
	filePath = file;

	sound_pacman_die = Mix_LoadWAV("sound_pacman_die.mp3");
	if (sound_pacman_die == NULL) {
		cout << "Failed to load pacman die sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
		return false;
	}

	sound_ghost_die = Mix_LoadWAV("sound_ghost_die.mp3");
	if (sound_ghost_die == NULL) {
		cout << "Failed to load ghost die sound effect! SDL_mixer Error: " << Mix_GetError() << endl;
		return false;
	}

	ghostClip[0].x = 0;
	ghostClip[0].y = 0;
	ghostClip[0].w = 20;
	ghostClip[0].h = 18;

	ghostClip[1].x = 20;
	ghostClip[1].y = 0;
	ghostClip[1].w = 20;
	ghostClip[1].h = 18;

	ghostClip[2].x = 20 * 2;
	ghostClip[2].y = 0;
	ghostClip[2].w = 20;
	ghostClip[2].h = 18;

	ghostClip[3].x = 20 * 3;
	ghostClip[3].y = 0;
	ghostClip[3].w = 20;
	ghostClip[3].h = 18;

	ghostClip[4].x = 20 * 4;
	ghostClip[4].y = 0;
	ghostClip[4].w = 20;
	ghostClip[4].h = 18;

	ghostClip[5].x = 20 * 5;
	ghostClip[5].y = 0;
	ghostClip[5].w = 20;
	ghostClip[5].h = 18;

	ghostClip[6].x = 20 * 6;
	ghostClip[6].y = 0;
	ghostClip[6].w = 20;
	ghostClip[6].h = 18;

	ghostClip[7].x = 20 * 7;
	ghostClip[7].y = 0;
	ghostClip[7].w = 20;
	ghostClip[7].h = 18;
	
	return setMode(NORMAL_MODE);
}

//Set the mode of the ghost
//NORMAL_MODE
//WEAKEN_MODE
//WEAKEN_ENDING_MODE
//DEAD_MODE
bool Ghost::setMode(int mode) {
	if (mode == NORMAL_MODE && !isGoingHome) {
		if (!ghostTexture.loadFromFile(filePath)) {
			std::cout << "Failed to load ghost normal texture!\n";
			return false;
		}
		MODE = NORMAL_MODE;
	}
	else if (mode == WEAKEN_MODE && !isAtHome() && MODE != DEAD_MODE) {
		if (!ghostTexture.loadFromFile("weakenghost.png")) {
			std::cout << "Failed to load ghost weaken texture!\n";
			return false;
		}
		MODE = WEAKEN_MODE;
		canReverse = true;
	}
	else if (mode == WEAKEN_ENDING_MODE && MODE != NORMAL_MODE && MODE != DEAD_MODE) {
		MODE = WEAKEN_ENDING_MODE;
	}
	else if (mode == DEAD_MODE) {
		if (!ghostTexture.loadFromFile("deadghost.png")) {
			std::cout << "Failed to load ghost weaken texture!\n";
			return false;
		}
		MODE = DEAD_MODE;
	}
	return true;
}

//Set the freeze frame time
void Ghost::setFrameTime(int frame) {
	frameTime = frame;
}

//Add the ghost to be reference later
void Ghost::addGhost(Ghost* _ghost) {
	ghost.push_back(_ghost);
}

//Render the Ghost
void Ghost::render(int frame, bool freeze) {
	//If game is not freezed then proceed
	if (!freeze) {
		//If ghost is going back home then continue move back to home
		if (isGoingHome) {
			backToHome();
			move();
		}
		//If it is within freeze time then decrease frame time by 1 and hold
		else if (frameTime > 0) {
			frameTime--;
		}
		//If it is at home then check is there any ghost with priority higher than itself
		//If true then back to home or stay at default position
		else if (isAtHome()) {
			bool backHome = false;
			int xDistance = 270 - x;

			for (unsigned int i = 0; i < ghost.size(); i++) {
				if (ghost[i]->isAtHome() && (getPriority() > ghost[i]->getPriority())) {
					backToHome();
					backHome = true;
					break;
				}
			}

			//If false then move out of the home box
			if (!backHome) {
				if (xDistance > 0) {
					NEXT_MOVE_X = GHOST_RIGHT;
				}
				else if (xDistance < 0) {
					NEXT_MOVE_X = GHOST_LEFT;
				}
				else if (xDistance == 0) {
					NEXT_MOVE_Y = GHOST_UP;
				}
			}
			move();
		}
		else {
			//If it is not weaken then chase the Pacman
			if (MODE != WEAKEN_MODE && MODE != WEAKEN_ENDING_MODE) {
				chasePacman();
			}
			move();
		}
	}
	
	//Render the ghost in different mode
	if (MODE == NORMAL_MODE) {
		ghostTexture.render(x, y, 30, 30, &ghostClip[(frame / 30) + DIRECTION]);
	}
	else if(MODE == WEAKEN_MODE) {
		ghostTexture.render(x, y, 30, 30, &ghostClip[frame / 30]);
	}
	else if (MODE == WEAKEN_ENDING_MODE) {
		ghostTexture.render(x, y, 30, 30, &ghostClip[4 - ((frame / 15) + 1)]);
	}
	else if (MODE == DEAD_MODE) {
		ghostTexture.render(x, y, 30, 30, &ghostClip[DIRECTION / 2]);
	}

	//Check is it colllided with the Pacman
	isPacmanCollide();
}

//Perform ghost movement
void Ghost::move() {
	bool isNextMove = false;

	//If there is next move then check is there any obstacle
	//If no then next direction become current direction
	switch (NEXT_MOVE_X) {
	case GHOST_RIGHT:
		x += VEL;
		if (!isObstacle()) {
			DIRECTION = NEXT_MOVE_X;
			isNextMove = true;
		}
		x -= VEL;
		break;

	case GHOST_LEFT:
		x -= VEL;
		if (!isObstacle()) {
			DIRECTION = NEXT_MOVE_X;
			isNextMove = true;
		}
		x += VEL;
		break;		
	}

	switch(NEXT_MOVE_Y) {
		case GHOST_UP:
			y -= VEL;
			if (!isObstacle()) {
				DIRECTION = NEXT_MOVE_Y;
				isNextMove = true;
			}
			y += VEL;
			break;

		case GHOST_DOWN:
			y += VEL;
			if (!isObstacle()) {
				DIRECTION = NEXT_MOVE_Y;
				isNextMove = true;
			}
			y -= VEL;
			break;
	}

	//If there is no next move then random a direction but no reverse direction
	if(!isNextMove){
		std::vector<int> way;

		y -= VEL;
		if (!isObstacle()) {
			way.push_back(GHOST_UP);
		}
		y += VEL * 2;
		if (!isObstacle()) {
			way.push_back(GHOST_DOWN);
		}
		y -= VEL;

		x -= VEL;
		if (!isObstacle()) {
			way.push_back(GHOST_LEFT);
		}
		x += VEL * 2;
		if (!isObstacle()) {
			way.push_back(GHOST_RIGHT);
		}
		x -= VEL;

		if (way.size() == 1) {
			DIRECTION = way[0];
		}
		else if (!way.empty()) {
			for (unsigned int i = 0; i < way.size(); i++) {
				if (way[i] == getOpposite()) {
					way.erase(way.begin() + i);
					break;
				}
			}

			srand(time(0));
			int selectedWay = rand() % way.size();
			DIRECTION = way[selectedWay];
		}
	}

	//If can reverse direction then reverse once
	if (canReverse) {
		DIRECTION = getOpposite();
		canReverse = false;
	}

	//Ghost move in the direction set
	switch (DIRECTION) {
	case GHOST_UP:
		y -= VEL;
		if (isObstacle()) {
			y += VEL;
		}
		break;
	case GHOST_DOWN:
		y += VEL;
		if (isObstacle()) {
			y -= VEL;
		}
		break;
	case GHOST_LEFT:
		x -= VEL;
		if (isObstacle()) {
			x += VEL;
		}
		break;
	case GHOST_RIGHT:
		x += VEL;
		if (isObstacle()) {
			x -= VEL;
		}
		break;
	}

	//Reset back to none
	NEXT_MOVE_X = NONE;
	NEXT_MOVE_Y = NONE;
}

//Find for the shortest path to chase the Pacman
void Ghost::chasePacman() {
	Path* start = new Path(x / 30, y / 30, 0, NULL, &map);
	Path* end = new Path(pacman->getX() / 30, pacman->getY() / 30, 0, NULL);
	vector<vector<int>> _ghost;
	for (unsigned int i = 0; i < ghost.size(); i++) {
		vector<int> temp;
		temp.push_back(ghost[i]->getX());
		temp.push_back(ghost[i]->getY());
		_ghost.push_back(temp);
	}
	start->ghost = _ghost;
	int nextX = NONE;
	int nextY = NONE;
	NEXT_MOVE_X = NONE;
	NEXT_MOVE_Y = NONE;

	chaseFrame--;

	//Find the path every 100 frame
	if (chaseFrame <= 0) {
		chasePath = start->findPath(end);
		chaseFrame = 100;
		chaseCounter = 1;
	}
	
	if (chasePath.size() == 1) {
		chaseCounter = 0;
	}

	//Perform the path return from the algorithm
	//Set next direction
	nextX = chasePath[chaseCounter][0] * 30;
	nextY = chasePath[chaseCounter][1] * 30;

	if (x == nextX && y == nextY) {
		chaseCounter++;
		if ((unsigned int) chaseCounter >= chasePath.size()) {
			chaseFrame = 0;
			nextX = NONE;
			nextY = NONE;
		}
		else {
			nextX = chasePath[chaseCounter][0] * 30;
			nextY = chasePath[chaseCounter][1] * 30;
		}
	}

	if (nextX == NONE && nextY == NONE) {
		return;
	}

	if (nextY - y < 0) {
		if (DIRECTION != GHOST_DOWN) {
			NEXT_MOVE_Y = GHOST_UP;
		}
	}
	else if (nextY - y > 0) {
		if (DIRECTION != GHOST_UP) {
			NEXT_MOVE_Y = GHOST_DOWN;
		}
	}
	if (nextX - x < 0) {
		if (DIRECTION != GHOST_RIGHT) {
			NEXT_MOVE_X = GHOST_LEFT;
		}
	}
	else if (nextX - x > 0) {
		if (DIRECTION != GHOST_LEFT) {
			NEXT_MOVE_X = GHOST_RIGHT;
		}
	}
}

//Find shortest path to back to the home box
void Ghost::backToHome() {
	Path* start = new Path(x / 30, y / 30, 0, NULL, &map);
	Path* end = new Path(xDefault / 30, yDefault / 30, 0, NULL);
	NEXT_MOVE_X = NONE;
	NEXT_MOVE_Y = NONE;
	chaseCounter = 0;

	//Set ghost isGoingHome flag
	if (!isGoingHome) {
		isGoingHome = true;
		backHomePath = start->findPath(end);
		backHomePathCounter = 1;
	}

	//If reached home then stop performing this function
	if (x == xDefault && y == yDefault) {
		isGoingHome = false;
		setMode(NORMAL_MODE);
		setFrameTime(180);
		return;
	}

	if (backHomePath.size() == 1) {
		backHomePathCounter = 0;
	}

	//Perform the path return from the algorithm
	//Set next direction
	int nextX = backHomePath[backHomePathCounter][0] * 30;
	int nextY = backHomePath[backHomePathCounter][1] * 30;

	if (x == nextX && y == nextY) {
		backHomePathCounter++;
		nextX = backHomePath[backHomePathCounter][0] * 30;
		nextY = backHomePath[backHomePathCounter][1] * 30;
	}

	if (nextY - y < 0) {
		NEXT_MOVE_Y = GHOST_UP;
	}
	else if (nextY - y > 0) {
		NEXT_MOVE_Y = GHOST_DOWN;
	}
	if (nextX - x < 0) {
		NEXT_MOVE_X = GHOST_LEFT;
	}
	else if (nextX - x > 0) {
		NEXT_MOVE_X = GHOST_RIGHT;
	}
}

//Check is it at the home box
bool Ghost::isAtHome() {
	if (x >= 240 && x <= 300 && y == 270 && MODE != DEAD_MODE) {
		return true;
	}
	return false;
}

//Check is there any obstacle at the current direction
bool Ghost::isObstacle() {
	//If exceed the left side screen then move to the right side
	//else if exceed the right side screen then move to the left side
	if (x <= -30 && y == 270) {
		x = 18 * 30;
		return false;
	}
	else if (x >= 570 && y == 270) {
		x = 0;
		return false;
	}
	//Check if there is a wall collision detection
	for (unsigned int row = 0; row < map.size(); row++) {
		for (unsigned int col = 0; col < map[row].size(); col++) {
			if (map[row][col] == "X") {
				if (checkCollision(col * 30, row * 30)) {
					return true;
				}
			}
		}
	}
	//Check if there is a ghost block its path
	if (MODE != DEAD_MODE && !isGoingHome) {
		for (unsigned int i = 0; i < ghost.size(); i++) {
			if (ghost[i]->getMode() != DEAD_MODE && !ghost[i]->isAtHome()) {
				if (checkCollision(ghost[i]->getX(), ghost[i]->getY())) {
					chaseCounter = 0;
					return true;
				}
			}
		}
	}

	return false;
}

//Check if ghost is collided with the Pacman
void Ghost::isPacmanCollide() {
	bool isCollide = checkCollision(pacman->getX() + 7, pacman->getY() + 7, 16, 16);

	//If Powerup then ghost dead
	//Else Pacman dead
	if (isCollide) {
		if (MODE == WEAKEN_MODE || MODE == WEAKEN_ENDING_MODE) {
			Mix_PlayChannel(PRIORITY, sound_ghost_die, 0);
			pacman->addScore(1000);
			setMode(DEAD_MODE);
			backToHome();
		}
		else if (MODE != DEAD_MODE) {
			Mix_PlayChannel(PRIORITY, sound_pacman_die, 0);
			pacman->life--;
			pacman->reset();
			reset();
			ghost[0]->reset();
			ghost[1]->reset();
		}
	}
}

//Check the collision
bool Ghost::checkCollision(int wallX, int wallY, int w, int h) {
	int top = y;
	int bottom = y + 30;
	int left = x;
	int right = x + 30;

	int wallTop = wallY;
	int wallBottom = wallY + h;
	int wallLeft = wallX;
	int wallRight = wallX + w;

	if (right <= wallLeft) {
		return false;
	}
	if (left >= wallRight) {
		return false;
	}
	if (bottom <= wallTop) {
		return false;
	}
	if (top >= wallBottom) {
		return false;
	}

	return true;
}

//Return the opposite direction to the current direction
int Ghost::getOpposite() {
	switch (DIRECTION){
	case GHOST_UP:
		return GHOST_DOWN;
	case GHOST_DOWN:
		return GHOST_UP;
	case GHOST_LEFT:
		return GHOST_RIGHT;
	case GHOST_RIGHT:
		return GHOST_LEFT;
	}
	return -1;
}

//Reset to the default
void Ghost::reset() {
	x = xDefault;
	y = yDefault;
	DIRECTION = GHOST_UP;
	frameTime = 60;
	chaseFrame = 0;
	NEXT_MOVE_X = NONE;
	NEXT_MOVE_Y = NONE;
	isGoingHome = false;
	canReverse = false;
	if (filePath.size() != 0) {
		setMode(NORMAL_MODE);
	}
}

//Destructor
Ghost::~Ghost() {
	free();
}

//Free memory resources
void Ghost::free() {
	ghostTexture.free();
	Mix_FreeChunk(sound_pacman_die);
	Mix_FreeChunk(sound_ghost_die);
	sound_pacman_die = NULL;
	sound_ghost_die = NULL;
}
//My Code End