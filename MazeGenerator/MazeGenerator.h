#pragma once
#include <vector>

namespace Maze {
	struct Tile {
		bool BottomWall = false, RightWall = false;
		unsigned int Set = -1;
	};
	std::vector<std::vector<Tile>>* GenerateMaze(unsigned int rowsAmount, unsigned int colsAmount, 
		unsigned int rightWallRand, unsigned int bottomWallRand);
}



