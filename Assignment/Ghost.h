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
const int WEAKEN_MODE_ENDING = 300;
const int DEAD_MODE = 400;

class Ghost {
public:
	Ghost();
	~Ghost();
	bool init(int priority, int xPos, int yPos, std::vector<std::vector<std::string>> _map, std::string filePath);
	void free();
	void addGhost(Ghost* _ghost);
	bool setMode(int mode);
	void setFrameTime(int frame);
	int getPriority() { return PRIORITY; };
	void backToHome();
	void move();
	bool isInHome();
	bool isWall();
	void isPacmanCollide();
	bool checkCollision(int wallX, int wallY, int w = 30, int h = 30);
	int getOpposite();
	int getX() { return x; };
	int getY() { return y; };
	void render(int frame);
	void chasePacman();
	void reset();
	Pacman* pacman;
	int MODE;
private:
	std::vector<Ghost*> ghost;
	int x, y, xDefault, yDefault, frameTime, chaseFrame;
	int vel;
	int NEXT_MOVE_X, NEXT_MOVE_Y, PRIORITY;
	int DIRECTION;
	bool isGoingHome, canReverse;
	int backHomePathCounter, chaseCounter;
	std::string filePath;
	LTexture ghostTexture;
	SDL_Rect ghostClip[8];
	std::vector<std::vector<std::string>> map;
	std::vector<std::vector<int>> backHomePath, chasePath;
	Mix_Chunk* sound_pacman_die = NULL;
	Mix_Chunk* sound_ghost_die = NULL;
};
#endif
