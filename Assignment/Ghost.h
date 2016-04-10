#pragma once

#ifndef __MONSTER_H
#define __MONSTER_H

#include <vector>
#include "Texture.h"
#include "Pacman.h"

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
	bool init(int xPos, int yPos, std::vector<std::vector<std::string>> _map, std::string filePath);
	void free();
	void addGhost(Ghost* _ghost);
	bool setMode(int mode);
	void setFrameTime(int frame);
	void backToHome();
	void move();
	bool isInHome();
	bool isWall();
	bool dead() { return isDead; };
	bool isPacmanCollide();
	bool checkCollision(int wallX, int wallY, int w = 30, int h = 30);
	int getOpposite();
	int getX() { return x; };
	int getY() { return y; };
	void render(int frame);
	Pacman* pacman;
private:
	std::vector<Ghost*> ghost;
	int x, y, xDefault, yDefault, frameTime;
	int vel;
	int DIRECTION;
	int MODE;
	int PREFERRED_X, PREFERRED_Y;
	bool isDead, canReverse;
	std::string filePath;
	LTexture ghostTexture;
	SDL_Rect ghostClip[8];
	std::vector<std::vector<std::string>> map;
};
#endif
