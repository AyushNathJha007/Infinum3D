#include "Window.h"
#include<sstream>
#include "resource.h" //For the icon stuff, so that we can bake it in
Window::WindowClass Window::WindowClass::WinClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr)) //GetModuleHandle() is used to get Handle to the instance, which is then saved in our data
{
	WNDCLASSEX WC = { 0 };

	WC.cbSize = sizeof(WC); //Allocate size to this structure
	WC.cbClsExtra = 0; //Number of extra bytes to be allocated to this class, stored at Windows API side -> We need none
	WC.cbWndExtra = 0; //Number of extra bytes to be allocated to each instance of Window created -> We need none
	WC.hCursor = nullptr; //Use default cursor icon
	WC.hIcon = static_cast<HICON>(LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,32,32,0)); //nullptr; //Use default Window icon
	WC.hbrBackground = nullptr; //No background drawn
	WC.lpfnWndProc = HandleMessageSetup; //Here, Windows Procedure is being pointed to HandleMessageSetup

	/*Why do we use this above?
	Because we can't register a member function directly with
	Windows API because it can't call stuffs like "HandleMessage"
	directly.
	So, what we do is we register a static function. This static function-
	1. Retrieves the pointer to our window class from the data stored in
	Windows 32 side.
	2. Forwards the message to Window Class Handler.
	
	So, long story short, we have got two static functions (HandleMessageSetup & HandleMessageThunk)
	which are registered with the Windows API side. The HandleMessageSetup is responsible for installation only,
	i.e, it only sets up a pointer to our instance in Windows 32 side. Then it installs the second handler
	(the Thunk one) which acts as an adapter; and adapts from WIN32 convention to C++ member call convention.*/

	WC.hInstance = GetInstance();
	WC.style = CS_OWNDC;
	WC.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0)); //nullptr;//Custom Icon for our application
	WC.lpszMenuName = nullptr; //We haven't got any menu. So no need.
	WC.lpszClassName = GetName();

	RegisterClassEx(&WC);	//Takes WindowsClassEx structure pointer as input
}

//Destructor to Unregister the WindowClass class
Window::WindowClass::~WindowClass()
{
	UnregisterClass(WinClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return WinClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return WinClass.hInst;
}

//Below function is for Construction of window 
Window::Window(int width, int height, const char* name) : width(width),height(height) //noexcept
{
	RECT winRegion;
	winRegion.left = 100;
	winRegion.right = width + winRegion.left;
	winRegion.top = 100;
	winRegion.bottom = height + winRegion.top;
	//AdjustWindowRect(&winRegion, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE); //This makes the client region of the desired size, rather than the complete window.

	//Check if AdjustWindowRect() failed and return error
	if (AdjustWindowRect(&winRegion, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)==0) //If AdjustWindowRect failed, it returns zero.
	{
		throw CHWND_LAST_EXCEPT();
	}
	//Test error thrown
	//throw CHWND_EXCEPT(ERROR_ARENA_TRASHED);

	//Create Window and get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		//"FakeClassName",name, ->Checking if exception handling works correctly.
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
		winRegion.right - winRegion.left, winRegion.bottom - winRegion.top,
		nullptr, nullptr, WindowClass::GetInstance(), this //through 'this' keyword, we will be able to access the information passed in here through the WM_NCCREATE.
		//Basically, 'this' keyword passes a lpointer (long pointer) to itself.
		//This is the way through which we connect the window instance to our Windows API Message Handling mechanism.
	);

	//Check if hWnd failed-> If yes, it will be null
	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	//Show Window
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	//Create GraphicsSetup object
	pGfx = std::make_unique<GraphicsSetup>(hWnd);

}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{

		throw CHWND_LAST_EXCEPT();
	}
}

LRESULT WINAPI Window::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) //WM_NCCREATE is the message sent prior to the creation of a new window
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam); //ReInterpret lParam to be a pointer to our CREATESTRUCT
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		//Now, we want to set the pointer to our window class inside of the window data on the Windows API side.
		//This will create a link between the window itself and the class that we are using to control it.
		//pWnd is the pointer to our Window.
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd)); //It allows to set data to the Windows API side with respect to the instance of the window
		//Through GWLP_USERDATA, we can set User Data associated to a particular window.

		//Since we have succesfully installed the pointer within our window, we want to use a different message procedure.
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));
		return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMessageThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd,GWLP_USERDATA));
		return pWnd->HandleMessage(hWnd, msg, wParam, lParam); //This forwards message to window class handler.
	}

	
LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0; //We return 0 because we want our Destructor to destroy the window, not DefProc.
	case WM_KILLFOCUS:
		kbd.ClearState(); //Resets all keystates when the focus from current window is lost for preventing zombie keystates
		break;
	/*----To Handle Keyboard Input------*/
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: //For handling keys like ALT and F10
		if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnabled())	//Here, we basically check bit 30 of keydown to be false.Bit 30 tells us about the previous state of the key.
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
	/*-----End Of Keyboard Input Handling------*/

	/*-----To Handle Mouse Input------*/
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		/*We do all stuffs related to capture of mouse here.
		If cursor is in client region -> The OnMouseMove() function is called as usual.
		And if mouse wasn't inside the clent area specifically before, we log an enter
		event and capture mouse.*/
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{

			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd); //This functions captures the mouse. It is a Windows API function.
				//This means that as long as we have mouse capture, even if the mouse leaves the
				//client region, we will be able to track it.
				mouse.OnMouseEnter();
			}
		}
		//when not in client -> log move and maintain capture if button is down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else
			{
				ReleaseCapture(); //Capture released if button isn't held up.
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftMBPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightMBPressed(pt.x, pt.y);
		break;
	}

	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftMBReleased(pt.x, pt.y);
		break;
	}

	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightMBReleased(pt.x, pt.y);
		break;
	}

	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		/*if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) //GETWheelDeltaParam is used to extract some values from wParam, which gives us some info about mouse scroll
		{
			mouse.OnWheelUp(pt.x, pt.y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(pt.x, pt.y);
		}*/
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelData(pt.x, pt.y, delta); //This will generate scroll up and down events as appropriate
		break;
	}

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// check for quit because peekmessage does not signal this via return val
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
			return msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR messages from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

GraphicsSetup& Window::Gfx()
{
	if (!pGfx)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

/*---------Exception Handling Part Starts Here--------------*/



std::string Window::Exception::TranslateErrorCode(HRESULT hResult) noexcept
{
	char* pMessageBuffer = nullptr;
	const DWORD nMessageLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),reinterpret_cast<LPSTR>(&pMessageBuffer),0,nullptr);
	//FormatMessage() takes the hResult and provides with a string description of the error.
	//ALLOCATE_BUFFER allocates us a buffer in the memory. We will pass a pointer(pMessageBuffer) to a pointer(pMessageBuffer)
	//which will point to a new buffer location, where the error code string is present.
	//Return value of FormatMessage() is the length of error string.
	if (nMessageLength == 0)
	{
		return "Error Code Unidentified!";
	}
	std::string ErrorString = pMessageBuffer; //Copy the error description present in pMessageBuffer to std::string
	LocalFree(pMessageBuffer); //Free the buffer allocated by windows
	return ErrorString;
}

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hResult(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Infinum Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hResult;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hResult);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Infinum Window Exception [No Graphics]";
}



