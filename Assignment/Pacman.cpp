#include "Pacman.h"
#include <sstream>

enum Pacman::Direction {
	UP, RIGHT, DOWN, LEFT
};

Pacman::Pacman() {
	score = 0;
	x = 9 * 30;
	y = 11 * 30;
	VEL = 1;
	angle = 0;
	flip = SDL_FLIP_NONE;
	DIRECTION = RIGHT;
	NEXT_DIRECTION = RIGHT;
	POWERUP = false;
	map = NULL;
}

bool Pacman::init(std::vector<std::vector<std::string>>* _map) {
	bool success = true;
	map = _map;

	if (!pacmanTexture.loadFromFile("pacman.png"))	{
		std::cout << "Failed to load pacman texture!\n";
		success = false;
	}
	else {
		pacmanClip[0].x = 0;
		pacmanClip[0].y = 0;
		pacmanClip[0].w = 82;
		pacmanClip[0].h = 82;

		pacmanClip[1].x = 83;
		pacmanClip[1].y = 0;
		pacmanClip[1].w = 82;
		pacmanClip[1].h = 82;

		pacmanClip[2].x = 85 * 2;
		pacmanClip[2].y = 0;
		pacmanClip[2].w = 82;
		pacmanClip[2].h = 82;

		pacmanClip[3].x = 85 * 3;
		pacmanClip[3].y = 0;
		pacmanClip[3].w = 82;
		pacmanClip[3].h = 82;

		pacmanClip[4].x = 87 * 4;
		pacmanClip[4].y = 0;
		pacmanClip[4].w = 82;
		pacmanClip[4].h = 82;

		pacmanClip[5].x = 0;
		pacmanClip[5].y = 84;
		pacmanClip[5].w = 82;
		pacmanClip[5].h = 82;

		pacmanClip[6].x = 82;
		pacmanClip[6].y = 84;
		pacmanClip[6].w = 82;
		pacmanClip[6].h = 82;
	}

	return success;
}

void Pacman::render(int frame) {
	move();
	pacmanTexture.render(x, y, 30, 30, &pacmanClip[frame], angle, NULL, flip);
}

void Pacman::handleEvent(SDL_Event e) {
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_UP:
		case SDLK_w:
			NEXT_DIRECTION = UP;
			break;

		case SDLK_RIGHT:
		case SDLK_d:
			NEXT_DIRECTION = RIGHT;
			break;

		case SDLK_DOWN:
		case SDLK_s:
			NEXT_DIRECTION = DOWN;
			break;

		case SDLK_LEFT:
		case SDLK_a:
			NEXT_DIRECTION = LEFT;
			break;
		}
	}
}

void Pacman::move() {
	if (isPowerUp()) {
		setPowerUp(false);
	}
	switch (NEXT_DIRECTION) {
	case UP:
		y -= VEL;
		if (!isWall()) {
			DIRECTION = NEXT_DIRECTION;
		}
		y += VEL;
		break;

	case RIGHT:
		x += VEL;
		if (!isWall()) {
			DIRECTION = NEXT_DIRECTION;
		}
		x -= VEL;
		break;

	case DOWN:
		y += VEL;
		if (!isWall()) {
			DIRECTION = NEXT_DIRECTION;
		}
		y -= VEL;
		break;

	case LEFT:
		x -= VEL;
		if (!isWall()) {
			DIRECTION = NEXT_DIRECTION;
		}
		x += VEL;
		break;
	}
	switch (DIRECTION) {
	case UP:
		angle = 270;
		flip = SDL_FLIP_NONE;
		y -= VEL;
		if (isWall()) {
			y += VEL;
		}
		else {
			eat();
		}
		break;

	case RIGHT:
		angle = 0;
		flip = SDL_FLIP_NONE;
		x += VEL;
		if (isWall()) {
			x -= VEL;
		}
		else {
			eat();
		}
		break;

	case DOWN:
		angle = 90;
		flip = SDL_FLIP_NONE;
		y += VEL;
		if (isWall()) {
			y -= VEL;
		}
		else {
			eat();
		}
		break;

	case LEFT:
		flip = SDL_FLIP_HORIZONTAL;
		angle = 0;
		x -= VEL;
		if (isWall()) {
			x += VEL;
		}
		else {
			eat();
		}
		break;
	}
}

bool Pacman::isWall() {
	if (x <= -30 && y == 270) {
		x = 18 * 30;
		return false;
	}
	else if (x >= 570 && y == 270) {
		x = 0;
		return false;
	}
	for (unsigned int row = 0; row < map->size(); row++) {
		for (unsigned int col = 0; col < (*map)[row].size(); col++) {
			if ((*map)[row][col] == "X") {
				if (checkCollision(col * 30, row * 30)) {
					return true;
				}
			}
		}
	}
	return false;
}

void Pacman::eat() {
	for (unsigned int row = 0; row < map->size(); row++) {
		for (unsigned int col = 0; col < (*map)[row].size(); col++) {
			if ((*map)[row][col] == "o") {
				if (checkCollision((col * 30) + 15, (row * 30) + 15, 0, 0)) {
					(*map)[row][col] = " ";
					score += 100;
					return;
				}
			}
			if ((*map)[row][col] == "p") {
				if (checkCollision((col * 30) + 15, (row * 30) + 15, 0, 0)) {
					(*map)[row][col] = " ";
					score += 500;
					setPowerUp(true);
					return;
				}
			}
		}
	}
}

//tobedeleted
//bool Pacman::eatPowerUp(std::vector<std::vector<std::string>>& map) {
//	/*for (unsigned int row = 0; row < map.size(); row++) {
//		for (unsigned int col = 0; col < map[row].size(); col++) {
//		if (checkCollision((powerUpCoor[i][0] * 30) + 15, (powerUpCoor[i][1] * 30) + 15, 0, 0)) {
//			powerUpCoor.erase(powerUpCoor.begin() + i);
//			score += 500;
//			return true;
//		}
//	}
//	return false;*/
//}

bool Pacman::checkCollision(int wallX, int wallY, int w, int h) {
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

void Pacman::setPosition(int xPos, int yPos) {
	x = xPos;
	y = yPos;
	DIRECTION = RIGHT;
	NEXT_DIRECTION = RIGHT;
}

std::string Pacman::getScore() {
	std::ostringstream temp;
	temp << score;
	return temp.str();
}

Pacman::~Pacman() {
	free();
}

void Pacman::free() {
	pacmanTexture.free();
}