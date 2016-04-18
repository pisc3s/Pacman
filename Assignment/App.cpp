#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include "Texture.h"
#include "Pacman.h"
#include "Ghost.h"
#include "Path.h"
using namespace std;

const int SCREEN_WIDTH = 19 * 30;
const int SCREEN_HEIGHT = 21 * 30;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* font = NULL;

Mix_Music* bgm = NULL;

LTexture scoreboard;
LTexture wall;
LTexture powerup;
LTexture life;
LTexture highScoreBoard;
LTexture boardText[4];
Pacman pacman;
Ghost redGhost;
Ghost blueGhost;
Ghost pinkGhost;
int pacmanFrame = 0;
int ghostFrame = 0;
int powerUpFrameTime = 0;
bool gameOver = false, getScore = false;
int alpha = 0;
vector<int> highscore;
vector<string> outputScore;
std::ostringstream fileText;

vector<vector<string>> map = {
	{ "X","X","X","X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X" },
	{ "X","o","o","o", "o", "o", "o", "o", "o", "X", "o", "o", "o", "o", "o", "o", "o", "o", "X" },
	{ "X","p","X","X", "o", "X", "X", "X", "o", "X", "o", "X", "X", "X", "o", "X", "X", "p", "X" },
	{ "X","o","o","o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "X" },
	{ "X","o","X","X", "o", "X", "o", "X", "X", "X", "X", "X", "o", "X", "o", "X", "X", "o", "X" },
	{ "X","o","o","o", "o", "X", "o", "o", "o", "X", "o", "o", "o", "X", "o", "o", "o", "o", "X" },
	{ "X","X","X","X", "o", "X", "X", "X", "o", "X", "o", "X", "X", "X", "o", "X", "X", "X", "X" },
	{ " "," "," ","X", "o", "X", "o", "o", "o", "o", "o", "o", "o", "X", "o", "X", " ", " ", " " },
	{ "X","X","X","X", "o", "X", "o", "X", "X", " ", "X", "X", "o", "X", "o", "X", "X", "X", "X" },
	{ "o","o","o","o", "o", "o", "o", "X", " ", " ", " ", "X", "o", "o", "o", "o", "o", "o", "o" },
	{ "X","X","X","X", "o", "X", "o", "X", "X", "X", "X", "X", "o", "X", "o", "X", "X", "X", "X" },
	{ " "," "," ","X", "o", "X", "o", "o", "o", " ", "o", "o", "o", "X", "o", "X", " ", " ", " " },
	{ "X","X","X","X", "o", "X", "o", "X", "X", "X", "X", "X", "o", "X", "o", "X", "X", "X", "X" },
	{ "X","o","o","o", "o", "o", "o", "o", "o", "X", "o", "o", "o", "o", "o", "o", "o", "o", "X" },
	{ "X","o","X","X", "o", "X", "X", "X", "o", "X", "o", "X", "X", "X", "o", "X", "X", "o", "X" },
	{ "X","p","o","X", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "X", "o", "p", "X" },
	{ "X","X","o","X", "o", "X", "o", "X", "X", "X", "X", "X", "o", "X", "o", "X", "o", "X", "X" },
	{ "X","o","o","o", "o", "X", "o", "o", "o", "X", "o", "o", "o", "X", "o", "o", "o", "o", "X" },
	{ "X","o","X","X", "X", "X", "X", "X", "o", "X", "o", "X", "X", "X", "X", "X", "X", "o", "X" },
	{ "X","o","o","o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "o", "X" },
	{ "X","X","X","X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X", "X" }
};

bool init();
bool loadMedia();
void close();
bool gameSetUp();
void handleFrame();
void renderMap();
void getLeaderboard();
void showLeaderboard();
bool descending(int i, int j) { return i > j; }

int main(int argc, char* args[]) {
	bool quit = false;
	SDL_Event e;

	if (!gameSetUp()) {
		return 0;
	}

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
				quit = true;
			}

			pacman.handleEvent(e);					
		}

		handleFrame();
		renderMap();

		if(gameOver){
			showLeaderboard();
		}
	}

	close();
	return 0;
}

bool gameSetUp() {
	if (!init()) {
		cout << "Failed to initialize!\n";
		return false;
	}

	if (!loadMedia()) {
		cout << "Failed to load media!\n";
		return false;
	}

	redGhost.pacman = &pacman;
	pinkGhost.pacman = &pacman;
	blueGhost.pacman = &pacman;

	redGhost.addGhost(&pinkGhost);
	redGhost.addGhost(&blueGhost);
	pinkGhost.addGhost(&redGhost);
	pinkGhost.addGhost(&blueGhost);
	blueGhost.addGhost(&redGhost);
	blueGhost.addGhost(&pinkGhost);

Mix_PlayMusic(bgm, -1);

return true;
}

void handleFrame() {
	if (powerUpFrameTime > 0) {
		if (powerUpFrameTime == 540) {
			redGhost.setMode(WEAKEN_MODE);
			pinkGhost.setMode(WEAKEN_MODE);
			blueGhost.setMode(WEAKEN_MODE);
		}
		else if (powerUpFrameTime == 180) {
			redGhost.setMode(WEAKEN_MODE_ENDING);
			pinkGhost.setMode(WEAKEN_MODE_ENDING);
			blueGhost.setMode(WEAKEN_MODE_ENDING);
		}
		else if (powerUpFrameTime == 1) {
			redGhost.setMode(NORMAL_MODE);
			pinkGhost.setMode(NORMAL_MODE);
			blueGhost.setMode(NORMAL_MODE);
		}
		powerUpFrameTime--;
	}

	if (pacmanFrame == 7 * 6 - 1) {
		pacmanFrame = 0;
	}
	else {
		pacmanFrame++;
	}

	if (ghostFrame == 59) {
		ghostFrame = 0;
	}
	else {
		ghostFrame++;
	}
}

void renderMap() {
	SDL_SetRenderDrawColor(gRenderer, 242, 252, 255, 255);
	SDL_RenderClear(gRenderer);

	for (unsigned int y = 0; y < map.size(); y++) {
		for (unsigned int x = 0; x < map[y].size(); x++) {
			int X = x * 30;
			int Y = y * 30;
			if (map[y][x] == "X") {
				wall.render(X, Y, 30, 30);
			}
			if (map[y][x] == "o") {
				SDL_SetRenderDrawColor(gRenderer, 255, 221, 0, 255);
				SDL_Rect fillRect = { X + 10, Y + 10, 10, 10 };
				SDL_RenderFillRect(gRenderer, &fillRect);
			}
			if (map[y][x] == "p") {
				powerup.render(X, Y, 30, 30);
			}
		}
	}

	if (!scoreboard.loadFromRenderedText(font, "Score: " + pacman.getScore(), { 0, 0, 0, 255 })) {
		cout << "Unable to render time texture!\n";
	}
	scoreboard.render(SCREEN_WIDTH - (scoreboard.getWidth() + 20), 0);

	pacman.render(pacmanFrame / 6);
	redGhost.render(ghostFrame);
	pinkGhost.render(ghostFrame);
	blueGhost.render(ghostFrame);

	vector<int> lifePos = { 30,60,90 };
	for (int i = 1; i <= pacman.life; i++) {
		life.render(lifePos[i - 1], 0, 30, 30);
	}

	if (pacman.isPowerUp()) {
		powerUpFrameTime = 540;
	}

	if (pacman.life == 0) {
		gameOver = true;
	}
	else {
		SDL_RenderPresent(gRenderer);
	}
}

void getLeaderboard() {
	highscore.clear();

	ofstream output;
	output.open("leaderboard.txt", fstream::app);
	if (output) {
		output << pacman.getScore() << endl;
	} else {
		cout << "File Error!" << endl;
	}
	output.close();
	output.clear();

	ifstream input;
	input.open("leaderboard.txt");

	while (true) {
		int temp;
		input >> temp;
		if (input.eof()) break;
		highscore.push_back(temp);
	}
	input.close();
	input.clear();

	sort(highscore.begin(), highscore.end(), descending);
}

void showLeaderboard(){
	if (!getScore) {
		getLeaderboard();
		getScore = true;
	}
	
	outputScore.clear();
	outputScore.push_back("High Score");
	for (unsigned int i = 0; i < highscore.size(); i++) {
		fileText << (i + 1) << ". " << highscore[i];
		outputScore.push_back(fileText.str());
		fileText.str("");
	}

	highScoreBoard.setAlpha(alpha);
	highScoreBoard.render(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 250, 300, 500);

	int winnerNumber = min((int) outputScore.size(), 4);

	for (unsigned int i = 0; i < winnerNumber; i++) {
		if (!boardText[i].loadFromRenderedText(font, outputScore[i], { 0, 0, 0, 255 })) {
			cout << "Unable to render board text texture!\n";
		}
		boardText[i].setAlpha(alpha);
		boardText[i].render(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 140 + (i * 60), boardText[i].getWidth(), boardText[i].getHeight());
	}

	alpha += 10;
	if(alpha > SDL_ALPHA_OPAQUE){
		alpha = 255;
	}

	SDL_RenderPresent(gRenderer);
}

bool init() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		cout << "Warning: Linear texture filtering not enabled!\n";
	}

	gWindow = SDL_CreateWindow("Pacman", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		cout << "Window could not be created! SDL Error: " << SDL_GetError() << endl;
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == NULL) {
		cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << endl;
		return false;
	}

	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
		return false;
	}

	if (TTF_Init() == -1) {
		cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError();
		return false;
	}

	return true;
}

bool loadMedia() {
	if (!wall.loadFromFile("wall.jpg")) {
		cout << "Failed to load wall texture!\n";
		return false;
	}

	if (!powerup.loadFromFile("powerup.png")) {
		cout << "Failed to load powerup texture!\n";
		return false;
	}

	if (!life.loadFromFile("life.png")) {
		cout << "Failed to load life texture!\n";
		return false;
	}

	if (!highScoreBoard.loadFromFile("board.png")) {
		cout << "Failed to load high score board texture!\n";
		return false;
	}
	highScoreBoard.setBlendMode(SDL_BLENDMODE_BLEND);
	boardText[0].setBlendMode(SDL_BLENDMODE_BLEND);
	boardText[1].setBlendMode(SDL_BLENDMODE_BLEND);
	boardText[2].setBlendMode(SDL_BLENDMODE_BLEND);
	boardText[3].setBlendMode(SDL_BLENDMODE_BLEND);

	bgm = Mix_LoadMUS("bgm.mp3");
	if (bgm == NULL) {
		cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
		return false;
	}

	font = TTF_OpenFont("PoiretOne.ttf", 25);
	if (font == NULL) {
		cout << "Failed to load true type font! SDL_ttf Error: " << TTF_GetError() << endl;
		return false;
	}

	return pacman.init(&map) && redGhost.init(1, 9, 9, map, "redghost.png") && 
		pinkGhost.init(2, 8, 9, map, "pinkghost.png") && blueGhost.init(3, 10, 9, map, "blueghost.png");
}

void close() {
	pacman.free();
	wall.free();
	scoreboard.free();
	life.free();
	powerup.free();
	highScoreBoard.free();
	boardText[0].free();
	boardText[1].free();
	boardText[2].free();
	boardText[3].free();

	redGhost.free();
	pinkGhost.free();
	blueGhost.free();

	Mix_FreeMusic(bgm);
	bgm = NULL;

	TTF_CloseFont(font);
	font = NULL;

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}