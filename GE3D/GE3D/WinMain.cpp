/*-----------The code commented out was just for exploration stuff-----------------*/

/*
#include<Windows.h> //All below symbols like hinstance,etc are defined in here
#include <sstream>
#include <string>

/*Used to create a custom Window Process
In this case, it has been created to define the property that quits out of the whole
application, rather than just closing the window, which happens with DefWinProc. 

LRESULT CALLBACK CustomWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)  //wParam-> Virtual KeyCode of the non-system key.
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(17);
		break;
	case WM_KEYDOWN:
		if (wParam == 'F') //Alphanumeric values doesn't have Virtual KeyCodes. They can be directly referred by their representation.
			SetWindowText(hwnd, "You pressed F");
		break;
	case WM_KEYUP:
		if (wParam == 'F')
			SetWindowText(hwnd, "Main Window");
		break;
	case WM_CHAR:
		{
			static std::string title;
			title.push_back((char)wParam);
			SetWindowText(hwnd, title.c_str());
		}
		break;
	case WM_LBUTTONDOWN:
		{
			/*The lParam for LMouseButton return the location where the button was clicked, relative to the top-left corner
			of the client region of window created. So, we use a POINTS structure to store those coordinates extracted from
			lParam (provided by winAPI).

			const POINTS pt = MAKEPOINTS(lParam);
			std::ostringstream oss;
			oss << "(" << pt.x << "," << pt.y << ")";
			SetWindowText(hwnd, oss.str().c_str());
		}
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/*What's the difference between WM_KEYDOWN and WM_CHAR?
WM_KEYDOWN is used to register keystrokes of all sort.
WM_CHAR is used when we enter alphanumeric data, especially words and sentences.*/

//WinMain Entry Point
/*CALLBACK is a modifier for the WinMain function, which tells C++ to use stdcall which passes parameters on the
stack; slightly different than usual calling convention - used since Windows API (which we are using here) uses
stdcall.

int CALLBACK WinMain(
	HINSTANCE hInstance, //Pointer which holds information about program like where it is loaded in memory, etc
	HINSTANCE hPrevInstance, //This is always NULL (hasn't changed since 1980s)
	LPSTR lpCmdLine, //We receive whatever is there in the command line as a single string through this
	int nCmdShow //Indicates how a window is loaded and displayed on startup
)
{
	const auto pClassName = "GE3D_class"; //Used in ClassName member of WC.

	//-------Step 1: Register Window Class -> This is done on Windows API side---------

	//WC is a configuration structure whose pointer reference will be passed into the RegisterClassEx()
	//Configuration Structure are also known as descriptors.

	WNDCLASSEX WC = { 0 }; 

	WC.cbSize = sizeof(WC); //Allocate size to this structure
	WC.cbClsExtra = 0; //Number of extra bytes to be allocated to this class, stored at Windows API side -> We need none
	WC.cbWndExtra = 0; //Number of extra bytes to be allocated to each instance of Window created -> We need none
	WC.hCursor = nullptr; //Use default cursor icon
	WC.hIcon = nullptr; //Use default Window icon
	WC.hbrBackground = nullptr; //No background drawn
	WC.lpfnWndProc = CustomWindowProc; //Use default Window Procedure
	WC.hInstance = hInstance;
	WC.style = CS_OWNDC;
	WC.hIconSm = nullptr;//Custom Icon for our application
	WC.lpszMenuName = nullptr; //We haven't got any menu. So no need.
	WC.lpszClassName = pClassName;

	RegisterClassEx( &WC );	//Takes WindowsClassEx structure pointer as input

	//------Step 2: Create Window Instance------

	/*We first store the handle that is returned by the CreateWindowEx() function ->
	We need the handle for future operations on Window.

	HWND hWnd = CreateWindowEx(
		0,pClassName,
		"Main Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, //Flag bits of all different menu styles clubbed together
		200,200,640,480,nullptr,nullptr,hInstance,nullptr
	);

	//Window created :) 
	//Now, we need to show the window.

	ShowWindow(hWnd, SW_SHOW);

	//while (true);

	//Window drawn here is completely static. This is because we are have not yet configured our window to handle messages.

	//-----Step 3: Message Processing Part-----
	MSG msg; //The message configuration structure
	BOOL getResult;
	while ((getResult=GetMessage(&msg, nullptr, 0, 0)) > 0) //Get Message returns 0 when message is to quit. It returns -1 for any errors.
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); //The message is sent to Window Process for further processing and stuff
	}

	//The below code monitors the exit code.
	if (getResult == -1)
	{
		return -1;
	}
	else
	{
		return msg.wParam; //wParam provided in PostQuitMessage() returns the exit code passed into it.
	}

	
}*/

/*-----Code for Application Starts Here----------*/

//Exception part has also been added in :)

#include "App.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	try {

		App{}.StartGameLoop();
		
	}
	catch (const ANJException &e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
		//A message box without a parent.
		//e.what() displays error description.
		//e.GetType() is used here to set the title of the message box.
		//We need an ok button and and exclamation icon.

	}
	catch (const std::exception &e)
	{
		MessageBox(nullptr, e.what(),"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	catch (...)
	{
		MessageBox(nullptr,"We don't know what went wrong!", "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}