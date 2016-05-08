//Code based on the lab example
#pragma once

#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

extern SDL_Renderer* gRenderer;

class LTexture
{
public:
	LTexture();
	~LTexture();
	bool loadFromFile(std::string path);
	bool loadFromRenderedText(TTF_Font* font, std::string textureText, SDL_Color textColor);
	void free();
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);
	//My Code Start - Method Overloading
	void render(int x, int y, int w, int h, SDL_Rect* clip = 0, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	//My Code End
	void render(int x, int y, SDL_Rect* clip = 0, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	int getWidth();
	int getHeight();
private:
	SDL_Texture* texture;
	int width;
	int height;
};

#endif
