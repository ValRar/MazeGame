#include "MazeGenerator.h"

using namespace Maze;

uint32_t counter = 0;
uint16_t colsAmount = 0, rowsAmount = 0;
uint16_t rightWallRandValue = 2, bottomWallRandValue = 2;
std::vector<std::vector<Tile>>* maze;


void PrepareRow(std::vector<Tile>& currentRow, std::vector<Tile>& oldRow) {
	for (int i = 0; i < currentRow.size(); i++) {
		if (oldRow[i].BottomWall) {
			currentRow[i].Set = ++counter;
		}
		else {
			currentRow[i].Set = oldRow[i].Set;
		}
	}
}
void UnionSets(std::vector<Tile>& row, uint32_t from, uint32_t to) {
	for (int i = 0; i < row.size(); i++) {
		if (row[i].Set == from)
			row[i].Set = to;
	}
}
int CountTilesWithoutBottomBorder(std::vector<Tile>& row, uint32_t set) {
	uint32_t counter = 0;
	for (int i = 0; i < row.size(); i++) {
		if (row[i].Set == set && !row[i].BottomWall) {
			counter++;
		}
	}
	return counter;
}
void PlaceRightWalls(std::vector<Tile>& row) {
	for (int i = 0; i < row.size() - 1; i++) {
		bool placeWall = rand() % rightWallRandValue;
		if (placeWall || row[i].Set == row[i + 1].Set) {
			row[i].RightWall = true;
		}
		else {
			UnionSets(row, row[i + 1].Set, row[i].Set);
		}
	}
}
void PlaceBottomWalls(std::vector<Tile>& row) {
	for (int i = 0; i < row.size(); i++)
	{
		bool placeWall = rand() % bottomWallRandValue;
		if (placeWall && CountTilesWithoutBottomBorder(row, row[i].Set) > 1) {
			row[i].BottomWall = true;
		}
	}
}
void InitializeVector(unsigned int rows, unsigned int cols) {
	maze = new std::vector<std::vector<Tile>>(rows);
	for (int i = 0; i < rows; i++) {
		(*maze)[i] = std::vector<Tile>(cols);
	}
}

std::vector<std::vector<Tile>>* Maze::GenerateMaze(unsigned int rowsAmount, unsigned int colsAmount, 
	unsigned int rightWallRand = 2, unsigned int bottomWallRand = 2) {
	rightWallRandValue = rightWallRand;
	bottomWallRandValue = bottomWallRand;
	InitializeVector(rowsAmount, colsAmount);

	for (int i = 0; i < colsAmount; i++) {
		(*maze)[0][i].Set = ++counter;

	}
	PrepareRow((*maze)[0], (*maze)[0]);
	PlaceRightWalls((*maze)[0]);
	PlaceBottomWalls((*maze)[0]);
	for (int i = 1; i < rowsAmount; i++) {
		PrepareRow((*maze)[i], (*maze)[i - 1]);
		PlaceRightWalls((*maze)[i]);
		PlaceBottomWalls((*maze)[i]);
	}
	for (int i = 0; i < colsAmount; i++) {
		(*maze)[rowsAmount - 1][i].BottomWall = true;
		if (i + 1 < colsAmount) {
			if ((*maze)[rowsAmount - 1][i].Set != (*maze)[rowsAmount - 1][i + 1].Set) {
				(*maze)[rowsAmount - 1][i].RightWall = false;
			}
			UnionSets((*maze)[rowsAmount - 1], (*maze)[rowsAmount - 1][i + 1].Set, (*maze)[rowsAmount - 1][i].Set);
		}
	}
	return maze;
}