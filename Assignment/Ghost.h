#pragma once

#ifndef __GHOST_H
#define __GHOST_H

#include <vector>
#include <SDL_mixer.h>
#include "Texture.h"
#include "Pacman.h"
#include "Path.h"

const int NONE = -1;
const int GHOST_UP = 0;
const int GHOST_DOWN = 2;
const int GHOST_LEFT = 4;
const int GHOST_RIGHT = 6;
const int NORMAL_MODE = 100;
const int WEAKEN_MODE = 200;
const int WEAKEN_ENDING_MODE = 300;
const int DEAD_MODE = 400;

class Ghost {
public:
	Pacman* pacman;

	Ghost();
	~Ghost();
	bool init(int priority, int xPos, int yPos, std::vector<std::vector<std::string>> _map, std::string filePath);
	void addGhost(Ghost* _ghost);
	bool setMode(int mode);
	bool isAtHome();
	int getX() { return x; };
	int getY() { return y; };
	int getPriority() { return PRIORITY; };
	int getMode() { return MODE; };
	void move();
	void render(int frame, bool freeze);
	void reset();
	void free();
private:
	std::vector<Ghost*> ghost;
	int x, y;
	int xDefault, yDefault;
	int VEL, DIRECTION;
	int NEXT_MOVE_X, NEXT_MOVE_Y;
	int PRIORITY, MODE;
	int frameTime, chaseFrame;
	int backHomePathCounter, chaseCounter;
	bool isGoingHome, canReverse;
	std::string filePath;
	LTexture ghostTexture;
	SDL_Rect ghostClip[8];
	std::vector<std::vector<std::string>> map;
	std::vector<std::vector<int>> backHomePath, chasePath;
	Mix_Chunk* sound_pacman_die = NULL;
	Mix_Chunk* sound_ghost_die = NULL;

	void setFrameTime(int frame);
	void backToHome();
	void chasePacman();
	bool isObstacle();
	void isPacmanCollide();
	bool checkCollision(int wallX, int wallY, int w = 30, int h = 30);
	int getOpposite();
};
#endif
