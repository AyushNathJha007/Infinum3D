#pragma once

/*Why this class??
Its obvious from the experiments that the Exception name and Exception Description thrown lack information
to track the root of the exception.
So, we tried to get those errors by creating the device on the DEBUG layer. But, looking at the debug console window repeatedly
to find the error isn't a great solution.
So, to show all that information directly in the message dialog box, we have an interface DXGIInfoQueue provided prehandedly.*/

#include "ANJWIN.h"
#include<wrl.h>
#include <vector>
#include<dxgidebug.h>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager() = default;
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long next = 0u;	//To store index of last message in queue, when set() is called
	
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};
