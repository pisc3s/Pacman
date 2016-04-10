#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Texture.h"

using namespace std;

LTexture::LTexture() {
	texture = NULL;
	width = 0;
	height = 0;
}

LTexture::~LTexture() {
	free();
}

bool LTexture::loadFromFile(string path) {
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if (loadedSurface == NULL) {
		cout << "Unable to load image! SDL_image Error: " << IMG_GetError() << endl;
	} else {
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);

		if (newTexture == NULL)	{
			cout << "Unable to create texture from " << path.c_str() <<	"! SDL Error: " << SDL_GetError() << endl;
		} else {
			width = loadedSurface->w;
			height = loadedSurface->h;
		}

		SDL_FreeSurface(loadedSurface);
	}

	texture = newTexture;
	return texture != NULL;
}

bool LTexture::loadFromRenderedText(TTF_Font* font, string textureText, SDL_Color textColor) {
	free();

	SDL_Surface* textSurface = TTF_RenderText_Blended(font, textureText.c_str(), textColor);
	if( textSurface != NULL ) {
		texture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if(texture == NULL ) {
			cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
		}
		else {
			width = textSurface->w;
			height = textSurface->h;
		}

		SDL_FreeSurface( textSurface );
	}
	else {
		cout << "Unable to render text surface! SDL_ttf Error: " <<	TTF_GetError() << endl;
	}

	return texture != NULL;
}

void LTexture::free() {
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	SDL_SetTextureColorMod(texture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	SDL_SetTextureBlendMode(texture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod(texture, alpha);
}

void LTexture::render(int x, int y, int w, int h, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderQuad = { x, y, w, h };

	SDL_RenderCopyEx(gRenderer, texture, clip, &renderQuad, angle, center, flip);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderQuad = { x, y, width, height };

	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(gRenderer, texture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
	return width;
}

int LTexture::getHeight() {
	return height;
}



