#include "Path.h"
#include <cmath>

Path::Path(int xPos, int yPos, int gCost, Path* parentPath, std::vector<std::vector<std::string>>* _map) {
	x = xPos;
	y = yPos;
	g = gCost;
	parent = parentPath;
	map = _map;
}

int Path::getF(Path* end) {
	return g + abs(x - end->x) + abs(y - end->y);
}

bool Path::isPath(Path* path) {
	for (unsigned int i = 0; i < closed.size(); i++) {
		if (closed[i]->x == path->x && closed[i]->y == path->y) {
			return false;
		}
	}
	for (unsigned int row = 0; row < map->size(); row++) {
		for (unsigned int col = 0; col < (*map)[row].size(); col++) {
			if (col == path->x && row == path->y && (*map)[row][col] != "X") {
				return true;
			}
		}
	}

	return false;
}

Path* Path::getOpenedPath(Path* path) {
	for (unsigned int i = 0; i < opened.size(); i++) {
		if (opened[i]->x == path->x && opened[i]->y == path->y) {
			return opened[i];
		}
	}
	return NULL;
}

void Path::addOpenedPath(Path* path, Path* parent) {
	Path* newPath = getOpenedPath(path);
	if (newPath == NULL) {
		newPath = path;
		newPath->g = parent->g + 10;
		newPath->parent = parent;
	}
	else {
		if (parent->g + 10 < newPath->g) {
			newPath->g = parent->g + 10;
			newPath->parent = parent;
		}
	}
	opened.push_back(newPath);
}

vector<vector<int>> Path::constructPath(Path* current) {
	vector<vector<int>> move;
	move.insert(move.begin(), { current->x, current->y });
	Path* prevMove = current->parent;

	while (prevMove != NULL) {
		move.insert(move.begin(), { prevMove->x, prevMove->y });
		prevMove = prevMove->parent;
	}

	return move;
}

vector<vector<int>> Path::findPath(Path* end) {
	clearAll();

	Path* current = this;
	opened.push_back(current);

	while (!opened.empty()) {
		current = opened[0];
		for (unsigned int i = 0; i < opened.size(); i++) {
			if (opened[i]->getF(end) < current->getF(end)) {
				current = opened[i];
			}
			if (opened[i]->getF(end) == current->getF(end)) {
				if (opened[i]->g <= current->g) {
					current = opened[i];
				}
			}
		}

		if (current->x == end->x && current->y == end->y) {
			return constructPath(current);
		}

		for (unsigned int i = 0; i < opened.size(); i++) {
			if (opened[i]->x == current->x && opened[i]->y == current->y) {
				opened.erase(opened.begin() + i);
				break;
			}
		}
		closed.push_back(current);

		int X = current->x;
		int Y = current->y;
		Path* up = new Path(X, Y - 1, 0, NULL);
		Path* down = new Path(X, Y + 1, 0, NULL);
		Path* left = new Path(X - 1, Y, 0, NULL);
		Path* right = new Path(X + 1, Y, 0, NULL);

		if (isPath(up)) { addOpenedPath(up, current); }
		if (isPath(down)) { addOpenedPath(down, current); }
		if (isPath(left)) { addOpenedPath(left, current); }
		if (isPath(right)) { addOpenedPath(right, current); }
	}

	return{ { -1, -1 } };
}

void Path::clearAll() {
	for (unsigned int i = 0; i < opened.size(); i++) {
		opened[i] = NULL;
	}
	opened.clear();

	for (unsigned int i = 0; i < closed.size(); i++) {
		closed[i] = NULL;
	}
	closed.clear();
}

Path::~Path() {
	parent = NULL;
	map = NULL;
	clearAll();
}