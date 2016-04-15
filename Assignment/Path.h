#pragma once

#ifndef __PATH_H
#define __PATH_H

#include <vector>
using namespace std;

class Path {
public:
	Path(int xPos, int yPos, int gCost, Path* parentPath, vector<vector<string>>* _map = NULL);
	~Path();
	int x, y, g;
	Path* parent;
	vector<vector<int>> findPath(Path* end);
	vector<vector<int>> ghost;
private:
	vector<vector<string>>* map;
	vector<Path*> opened;
	vector<Path*> closed;
	void clearAll();
	int getF(Path* end);
	bool isPath(Path* path);
	void addOpenedPath(Path* path, Path* parent);
	Path* getOpenedPath(Path* path);
	vector<vector<int>> constructPath(Path* current);
};
#endif
