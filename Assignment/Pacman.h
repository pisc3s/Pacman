#pragma once

#ifndef __PACMAN_H
#define __PACMAN_H

#include <SDL_mixer.h>
#include <vector>
#include "Texture.h"

class Pacman {
public:
	bool started;
	int life;

	Pacman();
	~Pacman();
	bool init(std::vector<std::vector<std::string>>* _map);
	void handleEvent(SDL_Event e);
	void render(int frame, bool freeze);
	int getX() { return x; };
	int getY() { return y; };
	std::string getScore();
	void addScore(int _score);
	bool isPowerUp() { return POWERUP; };
	void reset();
	void free();
private:
	enum Direction;
	int x, y, score;
	int VEL, DIRECTION, NEXT_DIRECTION;
	bool POWERUP;
	double angle;
	SDL_RendererFlip flip;
	Mix_Chunk* sound_food;
	Mix_Chunk* sound_powerup;
	LTexture pacmanTexture;
	SDL_Rect pacmanClip[7];
	std::vector<std::vector<std::string>>* map;

	void eat();
	void move();
	bool isObstacle();
	bool checkCollision(int wallX, int wallY, int w = 30, int h = 30);
	void setPowerUp(bool b) { POWERUP = b; };
};
#endif