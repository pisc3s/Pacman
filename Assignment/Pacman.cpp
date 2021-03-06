//My Code Start
#include "Pacman.h"
#include <sstream>

enum Pacman::Direction {
	UP, RIGHT, DOWN, LEFT
};

//Constructor
Pacman::Pacman() {
	VEL = 1;
	map = NULL;
	sound_food = NULL;
	sound_powerup = NULL;
	restart();
}

//Initialize everything and load all the media
//Return false if fails to load
bool Pacman::init(std::vector<std::vector<std::string>>* _map) {
	map = _map;

	if (!pacmanTexture.loadFromFile("pacman.png"))	{
		std::cout << "Failed to load pacman texture!\n";
		return false;
	}

	sound_powerup = Mix_LoadWAV("sound_powerup.mp3");
	if (sound_powerup == NULL) {
		std::cout << "Failed to load powerup sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return false;
	}

	sound_food = Mix_LoadWAV("sound_food.mp3");
	if (sound_food == NULL) {
		std::cout << "Failed to load food sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return false;
	}

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

	return true;
}

//Record the keyboard event
void Pacman::handleEvent(SDL_Event e) {
	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_UP:
		case SDLK_w:
			NEXT_DIRECTION = UP;
			started = true;
			break;

		case SDLK_RIGHT:
		case SDLK_d:
			NEXT_DIRECTION = RIGHT;
			started = true;
			break;

		case SDLK_DOWN:
		case SDLK_s:
			NEXT_DIRECTION = DOWN;
			started = true;
			break;

		case SDLK_LEFT:
		case SDLK_a:
			NEXT_DIRECTION = LEFT;
			started = true;
			break;
		}
	}
}

//Movement Function
void Pacman::move() {
	if (isPowerUp()) {
		setPowerUp(false);
	}

	//If there is next direction then check is there any obstacle
	//If it has no obstacle then it will become the current direction
	switch (NEXT_DIRECTION) {
	case UP:
		y -= VEL;
		if (!isObstacle()) {
			DIRECTION = NEXT_DIRECTION;
		}
		y += VEL;
		break;

	case RIGHT:
		x += VEL;
		if (!isObstacle()) {
			DIRECTION = NEXT_DIRECTION;
		}
		x -= VEL;
		break;

	case DOWN:
		y += VEL;
		if (!isObstacle()) {
			DIRECTION = NEXT_DIRECTION;
		}
		y -= VEL;
		break;

	case LEFT:
		x -= VEL;
		if (!isObstacle()) {
			DIRECTION = NEXT_DIRECTION;
		}
		x += VEL;
		break;
	}

	//Move to the direction set and with equivalent sprite animation
	switch (DIRECTION) {
	case UP:
		angle = 270;
		flip = SDL_FLIP_NONE;
		y -= VEL;
		if (isObstacle()) {
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
		if (isObstacle()) {
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
		if (isObstacle()) {
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
		if (isObstacle()) {
			x += VEL;
		}
		else {
			eat();
		}
		break;
	}
}

//Render the Pacman to the screen
void Pacman::render(int frame, bool freeze) {
	if (started && !freeze) {
		move();
	}
	pacmanTexture.render(x, y, 30, 30, &pacmanClip[frame], angle, NULL, flip);
}

//Eat pac-dots and the power pallets
void Pacman::eat() {
	for (unsigned int row = 0; row < map->size(); row++) {
		for (unsigned int col = 0; col < (*map)[row].size(); col++) {
			//Detect is it collided with the pac-dots
			//Add 100 score if true and erase the dot from the map
			if ((*map)[row][col] == "o") {
				if (checkCollision((col * 30) + 15, (row * 30) + 15, 0, 0)) {
					Mix_PlayChannel(-1, sound_food, 0);
					(*map)[row][col] = " ";
					score += 100;
					return;
				}
			}

			//Detect is it collided with the power pallets
			//Add 500 score if true and erase the power palletes from the map
			if ((*map)[row][col] == "p") {
				if (checkCollision((col * 30) + 15, (row * 30) + 15, 0, 0)) {
					Mix_PlayChannel(-1, sound_powerup, 0);
					(*map)[row][col] = " ";
					score += 500;
					setPowerUp(true);
					return;
				}
			}
		}
	}
}

//Check that is there any obstacle at the current position
bool Pacman::isObstacle() {
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

//Collision detection
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

//Return current score
std::string Pacman::getScore() {
	std::ostringstream temp;
	temp << score;
	return temp.str();
}

//Add score to current score
void Pacman::addScore(int _score) {
	score += _score;
}

//Reset everything to default
void Pacman::reset() {
	x = 9 * 30;
	y = 11 * 30;
	angle = 0;
	flip = SDL_FLIP_NONE;
	DIRECTION = RIGHT;
	NEXT_DIRECTION = RIGHT;
	POWERUP = false;
	started = false;
}

//Restart the game
void Pacman::restart() {
	score = 0;
	life = 3;
	reset();
}

//Destructor
Pacman::~Pacman() {
	free();
}

//Free memory resources
void Pacman::free() {
	pacmanTexture.free();
	Mix_FreeChunk(sound_food);
	Mix_FreeChunk(sound_powerup);
	sound_food = NULL;
	sound_powerup = NULL;
}
//My Code End