#pragma once

#ifndef __PACMAN_H
#define __PACMAN_H

#include <vector>
#include "Texture.h"

class Pacman {
public:
	Pacman();
	~Pacman();
	bool init(std::vector<std::vector<std::string>>* _map);
	void free();
	void handleEvent(SDL_Event e);
	void move();
	bool isWall();
	void eat();
	bool checkCollision(int wallX, int wallY, int w = 30, int h = 30);
	void render(int frame);
	void setPosition(int xPos, int yPos);
	int getX() { return x; };
	int getY() { return y; };
	std::string getScore();
	bool isPowerUp() { return POWERUP; };
	void setPowerUp(bool b) { POWERUP = b; };
private:
	int x, y;
	enum Direction;
	int VEL, DIRECTION, NEXT_DIRECTION;
	double angle;
	int score;
	bool POWERUP;
	SDL_RendererFlip flip;
	LTexture pacmanTexture;
	SDL_Rect pacmanClip[7];
	std::vector<std::vector<std::string>>* map;
};
#endif