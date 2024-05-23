// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for operator+, to_string
#include <Windows.h>

#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, gauge, text, Element, operator|, vbox, border
#include "MazeGenerator.h"
#include "Player.hpp"
#include "Timer.hpp"
using namespace ftxui;

const int blockWidth = 5, fullWidth = blockWidth + 1;
bool showGameComponents = false;


Canvas* renderMaze(std::vector<std::vector<Maze::Tile>>* mazePtr) {
	int rows = mazePtr->size(), columns = (*mazePtr)[0].size();
	Canvas* c = new Canvas(columns * fullWidth + 2, rows * fullWidth + 4);
	c->DrawPointLine(0, 0, 0, rows * fullWidth);
	c->DrawPointLine(0, 0, columns * fullWidth, 0);
	c->DrawPointLine(columns * fullWidth, 0, columns * fullWidth, rows * fullWidth);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++)
		{
			if ((*mazePtr)[i][j].RightWall)
				c->DrawPointLine((j + 1) * fullWidth, i * fullWidth, (j + 1) * fullWidth, (i + 1) * fullWidth);
			if ((*mazePtr)[i][j].BottomWall)
				c->DrawPointLine(j * fullWidth, (i + 1) * fullWidth, (j + 1) * fullWidth, (i + 1) * fullWidth);
		}
	}
	return c;
}
void renderMarker(Canvas* canvas, Point<int>& point, Color color) {
	canvas->DrawPointCircleFilled(point.x * fullWidth + fullWidth / 2, point.y * fullWidth + fullWidth / 2, 1, color);
}

Canvas DrawPlayer(Canvas c, Point<double>& point) {
	c.DrawBlockCircleFilled(point.x * fullWidth + fullWidth / 2, point.y * fullWidth + fullWidth / 2, 1);
	return std::move(c);
}

bool isPlayerWon(Player* player, Point<int>& mazeEnd) {
	return floor(player->GetCurrentX()) == mazeEnd.x && floor(player->GetCurrentY()) == mazeEnd.y;
}

void GameLoop() {
	int score = 0;
	int rows = 20, columns = 20;
	Timer mazeTimer, totalTimer;
	Point<int> startPoint(0, rand() % rows), endPoint(columns - 1, rand() % rows);
	auto maze = std::unique_ptr<std::vector<std::vector<Maze::Tile>>>(Maze::GenerateMaze(rows, columns, 2, 2));
	auto c = std::unique_ptr<Canvas>(renderMaze(maze.get()));
	renderMarker(c.get(), endPoint, Color::Green);
	auto player = std::make_unique<Player>(Player(startPoint.x, startPoint.y));
	auto renderer = Renderer([&] {
		auto playerPoint = player->GetCurrentPoint();
		auto cWithPlayer = DrawPlayer(*c, playerPoint);
		return hbox({ canvas(cWithPlayer) | border, vbox({
					text("Score: " + std::to_string(score)),
					text("Current level time: " + mazeTimer.GetFormattedString()),
					text("Total time: " + totalTimer.GetFormattedString())
				}) | border
			}) | border;
		});
	renderer |= CatchEvent([&](Event e) {
		int x = player->GetTargetX();
		int y = player->GetTargetY();
		if (e == e.ArrowRight && x < columns - 1 && !(*maze)[y][x].RightWall)
			player->Move(1, 0);
		else if (e == e.ArrowLeft && x > 0 && !(*maze)[y][x - 1].RightWall)
			player->Move(-1, 0);
		else if (e == e.ArrowDown && y < rows - 1 && !(*maze)[y][x].BottomWall)
			player->Move(0, 1);
		else if (e == e.ArrowUp && y > 0 && !(*maze)[y - 1][x].BottomWall)
			player->Move(0, -1);
		return false;
		});
	auto screen = ScreenInteractive::FitComponent();
	std::thread renderCallingThread([&] {
		while (true)
		{
			if (isPlayerWon(player.get(), endPoint)) {
				maze = std::unique_ptr<std::vector<std::vector<Maze::Tile>>>(Maze::GenerateMaze(rows, columns, 2, 2));
				c = std::unique_ptr<Canvas>(renderMaze(maze.get()));
				endPoint = Point<int>(columns - 1, rand() % rows);
				startPoint = Point<int>(0, rand() % rows);
				player = std::make_unique<Player>(Player(startPoint.x, startPoint.y));
				renderMarker(c.get(), endPoint, Color::Green);
				score++;
				mazeTimer.Reset();
			}
			screen.PostEvent(Event::Custom);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		});
	screen.Loop(renderer);
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	srand(time(0));
	GameLoop();
	return 0;
}