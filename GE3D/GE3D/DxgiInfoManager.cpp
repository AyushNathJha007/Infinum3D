#include "DxgiInfoManager.h"
#include "Window.h"
#include "GraphicsSetup.h"
#include <dxgidebug.h>
#include <memory>
#include "GraphicsSetupThrowsMacro.h"

#pragma comment(lib, "dxguid.lib")



//DXGI_DEBUG_ALL gives all debug messages

DxgiInfoManager::DxgiInfoManager()
{
	// define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void **); //To get all the info, we need this function "DXGIDebugInterface"

	// Since the interface is in another dll, we load the dll that contains the function DXGIGetDebugInterface
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	// get address of DXGIGetDebugInterface in dll
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	if (DxgiGetDebugInterface == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	HRESULT HR;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), (&pDxgiInfoQueue))); //Call The function to get the interface
}



void DxgiInfoManager::Set() noexcept
{
	// set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL); //Next will be set to the index of next message that will go in queue
	//that,is, currentLastMessage+1
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	//Loop through the messages in the queue(fron next to end)
	for (auto i = next; i < end; i++)
	{
		HRESULT HR;
		SIZE_T messageLength;
		// get the size of message i in bytes
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength)); //This will fill the messageLength with the length of message at that index
		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);	//Once we have size of the index, we allocate a buffer
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// get the message and push its description into the vector
		GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength)); //We call GetMessage again to get a pointer to that buffer this time
		messages.emplace_back(pMessage->pDescription); //We take the description part of the message and put it in a vector of strings
	}
	return messages;
	/*If we use only GetMessages() it will return all the messages from the beginning of application start(everytime). So,
	we have a Set() function defined above, that will return messages generated only after the last call to Set().*/
}