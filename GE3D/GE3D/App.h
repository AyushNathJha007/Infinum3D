#pragma once
#include "Window.h"
#include "InfinumTimer.h"

class App
{
public:
	App();
	int StartGameLoop(); //Called when application will start; basically to start the game loop
	~App();
private:
	void DoFrame(); //Logic for each frame will be crammed in here
private:
	Window window;
	InfinumTimer timer;
	std::vector<std::unique_ptr<class Box>> boxes;
};
