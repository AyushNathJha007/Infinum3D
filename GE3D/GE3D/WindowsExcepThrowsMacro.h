#pragma once
//Error exception helper macro
#define CHWND_EXCEPT(hResult) Window::HrException(__LINE__,__FILE__,hResult)

//__LINE__ and __FILE__ macros are required to be invoked to get error line no. and the file. This little macro helps.

//This helper macro displays details from the last HRESULT.
#define CHWND_LAST_EXCEPT() Window::HrException(__LINE__,__FILE__,GetLastError())
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )
