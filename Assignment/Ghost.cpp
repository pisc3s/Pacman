#include "Ghost.h"
#include <stdlib.h>
#include <ctime>

Ghost::Ghost() {
	vel = 1;
	DIRECTION = GHOST_UP;
	frameTime = 0;
	NEXT_MOVE_X = NONE;
	NEXT_MOVE_Y = NONE;
	pacman = NULL;
	isGoingHome = false;
}

bool Ghost::init(int priority, int xPos, int yPos, std::vector<std::vector<std::string>> _map, std::string file) {
	PRIORITY = priority;
	x = xPos * 30;
	y = yPos * 30;
	xDefault = x;
	yDefault = y;
	map = _map;
	filePath = file;

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

bool Ghost::setMode(int mode) {
	if (mode == NORMAL_MODE && !isGoingHome) {
		if (!ghostTexture.loadFromFile(filePath)) {
			std::cout << "Failed to load ghost normal texture!\n";
			return false;
		}
		MODE = NORMAL_MODE;
	}
	else if (mode == WEAKEN_MODE) {
		if (!ghostTexture.loadFromFile("weakenghost.png")) {
			std::cout << "Failed to load ghost weaken texture!\n";
			return false;
		}
		MODE = WEAKEN_MODE;
	}
	else if (mode == WEAKEN_MODE_ENDING) {
		MODE = WEAKEN_MODE_ENDING;
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

void Ghost::setFrameTime(int frame) {
	frameTime = frame;
}

void Ghost::addGhost(Ghost* _ghost) {
	ghost.push_back(_ghost);
}

void Ghost::render(int frame) {
	if (isGoingHome) {
		backToHome();
		move();
	} else if (frameTime > 0) {
		frameTime--;
	} else if (isInHome()) {
		bool backHome = false;
		int xDistance = 270 - x;

		for (unsigned int i = 0; i < ghost.size(); i++) {
			if (ghost[i]->isInHome() && (getPriority() > ghost[i]->getPriority())) {
				backToHome();
				backHome = true;
				break;
			}
		}

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
	} else {
		move();
	}
	
	if (MODE == NORMAL_MODE) {
		ghostTexture.render(x, y, 30, 30, &ghostClip[frame + DIRECTION]);
	}
	else if(MODE == WEAKEN_MODE) {
		ghostTexture.render(x, y, 30, 30, &ghostClip[frame]);
	}
	else if (MODE == WEAKEN_MODE_ENDING) {
		ghostTexture.render(x, y, 30, 30, &ghostClip[4 - (frame + 1)]);
	}
	else if (MODE == DEAD_MODE) {
		ghostTexture.render(x, y, 30, 30, &ghostClip[DIRECTION / 2]);
	}
}

void Ghost::move() {
	bool isNextMove = false;
	switch (NEXT_MOVE_X) {
	case GHOST_RIGHT:
		x += vel;
		if (!isWall()) {
			DIRECTION = NEXT_MOVE_X;
			isNextMove = true;
		}
		x -= vel;
		break;

	case GHOST_LEFT:
		x -= vel;
		if (!isWall()) {
			DIRECTION = NEXT_MOVE_X;
			isNextMove = true;
		}
		x += vel;
		break;		
	}

	switch(NEXT_MOVE_Y) {
		case GHOST_UP:
			y -= vel;
			if (!isWall()) {
				DIRECTION = NEXT_MOVE_Y;
				isNextMove = true;
			}
			y += vel;
			break;

		case GHOST_DOWN:
			y += vel;
			if (!isWall()) {
				DIRECTION = NEXT_MOVE_Y;
				isNextMove = true;
			}
			y -= vel;
			break;
	}

	if(!isNextMove){
		std::vector<int> way;

		y -= vel;
		if (!isWall()) {
			way.push_back(GHOST_UP);
		}
		y += vel * 2;
		if (!isWall()) {
			way.push_back(GHOST_DOWN);
		}
		y -= vel;

		x -= vel;
		if (!isWall()) {
			way.push_back(GHOST_LEFT);
		}
		x += vel * 2;
		if (!isWall()) {
			way.push_back(GHOST_RIGHT);
		}
		x -= vel;

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

	switch (DIRECTION) {
	case GHOST_UP:
		y -= vel;
		if (isWall()) {
			y += vel;
		}
		break;
	case GHOST_DOWN:
		y += vel;
		if (isWall()) {
			y -= vel;
		}
		break;
	case GHOST_LEFT:
		x -= vel;
		if (isWall()) {
			x += vel;
		}
		break;
	case GHOST_RIGHT:
		x += vel;
		if (isWall()) {
			x -= vel;
		}
		break;
	}
	NEXT_MOVE_X = NONE;
	NEXT_MOVE_Y = NONE;
}

void Ghost::backToHome() {
	Path* start = new Path(x / 30, y / 30, 0, NULL, &map);
	Path* end = new Path(xDefault / 30, yDefault / 30, 0, NULL);
	NEXT_MOVE_X = NONE;
	NEXT_MOVE_Y = NONE;
	if (!isGoingHome) {
		isGoingHome = true;
		path = start->findPath(end);
		counter = 1;
	}
	if (x == xDefault && y == yDefault) {
		isGoingHome = false;
		setMode(NORMAL_MODE);
		setFrameTime(180);
		return;
	}

	if (path.size() == 1) {
		counter = 0;
	}

	int nextX = path[counter][0] * 30;
	int nextY = path[counter][1] * 30;

	if (x == nextX && y == nextY) {
		counter++;
		nextX = path[counter][0] * 30;
		nextY = path[counter][1] * 30;
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

bool Ghost::isInHome() {
	if (x >= 240 && x <= 300 && y >= 211 && y <= 270) {
		return true;
	}
	return false;
}

bool Ghost::isWall() {
	if (x <= -30 && y == 270) {
		x = 18 * 30;
		return false;
	}
	else if (x >= 570 && y == 270) {
		x = 0;
		return false;
	}
	for (unsigned int row = 0; row < map.size(); row++) {
		for (unsigned int col = 0; col < map[row].size(); col++) {
			if (map[row][col] == "X") {
				if (checkCollision(col * 30, row * 30)) {
					return true;
				}
			}
		}
	}
	if (MODE != DEAD_MODE && !isGoingHome) {
		for (unsigned int i = 0; i < ghost.size(); i++) {
			if (ghost[i]->MODE != DEAD_MODE && !ghost[i]->isInHome()) {
				if (checkCollision(ghost[i]->getX(), ghost[i]->getY())) {
					return true;
				}
			}
		}
	}

	return false;
}

bool Ghost::isPacmanCollide() {
	return checkCollision(pacman->getX() + 7, pacman->getY() + 7, 16, 16);
}

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

Ghost::~Ghost() {
	free();
}

void Ghost::free() {
	ghostTexture.free();
}