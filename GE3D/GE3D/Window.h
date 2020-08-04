/*A class to represent a window, which encapsulates stuffs related to construction, destruction
of window as well as Message Handling. It will also encapsulate handle to the window, as well as
the operations performed on the handle. :)*/

#pragma once

#include "ANJWIN.h"
#include "ANJException.h"
#include "Input_Keyboard.h"
#include "Input_Mouse.h"
#include "GraphicsSetup.h"
#include<memory>
#include<optional>
#include "WindowsExcepThrowsMacro.h"

//Below interfaces are defined which do all the cool stuff related to window.

class Window
{
	//For handling exception stuff -> Inherited from ANJException
	//It's an inner class of Window. Hence Window exception.
public:
	class Exception :public ANJException	//Base Exception Class
	{
		using ANJException::ANJException;
	public:
		static std::string TranslateErrorCode(HRESULT hResult) noexcept; //Takes in Windows Error Code and returns a string description of the error.
	};
	class HrException : public Exception	//Hr Exception class inherited from base Exception class above
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hResult;

	};
	class NoGfxException : public Exception //No GFX Exception class also inherited from base class above
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};
private:
	//A singleton to manage the contruction/destruction of the window class
	/*What's a singleton class by the way?->
	It's a creational design pattern which ensures that only one object of its
	kind exists and provides a single point of access to it for any other piece of code.*/

	/*The noexcept operator performs a compile-time check that returns true if an 
	expression is declared to not throw any exceptions.*/

	class WindowClass
	{
	public: 
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept; //Constructor that registers the window class
		~WindowClass(); //Destruction of the window class(unregistering)
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* WinClassName = "GEFS (Direct 3D) Window";
		static WindowClass WinClass;
		HINSTANCE hInst;

	};
	//Singleton Ends Here
public:
	Window(int width, int height, const char* name); //noexcept; //Constructor which constructs the window
	//noexcept has been commented because now we are configuring it to throw exceptions.
	~Window(); //To destroy the window
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title); //This will allow hWnd to be private and encapsulated in our class
	static std::optional<int> ProcessMessages() noexcept;	//static, because we want to process messages for all windows that exist
	//<optional> is C++ 17 stuff, which allows to optionally return int.
	GraphicsSetup& Gfx();
private:
	static LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMessageThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	int height;
	int width;
	HWND hWnd;
	std::unique_ptr<GraphicsSetup> pGfx; //We define a pointer here, because we need the window Handler (hWnd) for this
};


