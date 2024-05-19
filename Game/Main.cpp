// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>  // for shared_ptr, __shared_ptr_access
#include <string>  // for operator+, to_string

#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, gauge, text, Element, operator|, vbox, border
#include "MazeGenerator.h"
#include "Player.hpp"
using namespace ftxui;

// This is a helper function to create a button with a custom style.
// The style is defined by a lambda function that takes an EntryState and
// returns an Element.
// We are using `center` to center the text inside the button, then `border` to
// add a border around the button, and finally `flex` to make the button fill
// the available space.

const int blockWidth = 5, fullWidth = blockWidth + 1;


Canvas* renderMaze(std::vector<std::vector<Maze::Tile>>* mazePtr) {
	int rows = mazePtr->size(), columns = (*mazePtr)[0].size();
	Canvas* c = new Canvas((rows + 1) * fullWidth, (columns + 1) * fullWidth);
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

Player player(0, 0);

Canvas DrawPlayer(Canvas c, double row, double column) {
	c.DrawBlockCircleFilled(column * fullWidth + fullWidth / 2, row * fullWidth + fullWidth / 2, 1);
	return std::move(c);
}

int main() {
	int rows = 20, columns = 20;
	srand(time(0));
	auto maze = std::unique_ptr<std::vector<std::vector<Maze::Tile>>>(Maze::GenerateMaze(rows, columns, 2, 2));
	auto c = renderMaze(maze.get());
	auto renderer = Renderer([&] {
		auto c1 = DrawPlayer(*c, player.GetCurrentY(), player.GetCurrentX());
		return canvas(c1);
		});
	renderer |= CatchEvent([&](Event e) {
		int x = player.GetTargetX();
		int y = player.GetTargetY();
		if (e == e.ArrowRight && x < columns - 1 && !(*maze)[y][x].RightWall)
			player.Move(1, 0);
		else if (e == e.ArrowLeft && x > 0 && !(*maze)[y][x - 1].RightWall)
			player.Move(-1, 0);
		else if (e == e.ArrowDown && y < rows - 1 && !(*maze)[y][x].BottomWall)
			player.Move(0, 1);
		else if (e == e.ArrowUp && y > 0 && !(*maze)[y - 1][x].BottomWall)
			player.Move(0, -1);
		return false;
		});
	auto screen = ScreenInteractive::FitComponent();
	std::thread renderCallingThread([&] {
		while (true)
		{
			screen.PostEvent(Event::Custom);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		});
	screen.Loop(renderer);
	return 0;
}