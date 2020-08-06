#include "App.h"
#include<sstream>
#include<iomanip>
#include "Box.h"
#include <memory>

App::App() :window(800, 600, "Infinum 3D") //For creating the app, we create a window
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 80; i++)
	{
		boxes.push_back(std::make_unique<Box>(
			window.Gfx(), rng, adist,
			ddist, odist, rdist
			));
	}
	window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{}


int App::StartGameLoop()	//We basically put all message logic in here
{
	/*Well, why do we use PeekMessage and what's the problem with GetMessage()?
	When GetMessage() finds no message in queue, it blocks and waits for message 
	to enter into the queue, as a result, the application sleeps until the message 
	comes in again.
	PeekMessage can copy a message from message queue without actually removing it.
	But, another function that we wanna make use of through PeakMessage is its ability to return immediately when no
	message is there in the queue.
	So, we want to use PM_REMOVE, since we want to read the message from the queue, remove it just
	like what GetMessage does, and then return immediately in case there's no message in the queue.
	
	Another difference between PeekMessage and GetMessage is that the return value of PeekMessage is BOOL.
	It returns non zero val when message is available, and zero when message is not available.*/

	/*MSG msg;
	BOOL getResult;
	while ((getResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame();
	}
	
	if (getResult == -1)
	{
		throw CHWND_LAST_EXCEPT();
	}

	//wParam is passed to PostQuitMessage()
	return msg.wParam;*/

	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
		Sleep(1);
	}
}

void App::DoFrame() //Contains top level of the game logic
{
	auto dt = timer.Mark();
	window.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& b : boxes)
	{
		b->Update(dt);
		b->Draw(window.Gfx());
	}
	window.Gfx().EndFrame();
}
	