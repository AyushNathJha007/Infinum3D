#include "ANJException.h"
#include<sstream>

ANJException::ANJException(int line, const char* file) noexcept :
	line(line), file(file)
{

}

/*what() doesn't return a string. It returns a char pointer. So, if we use the ostringstream here
with the data given by what(), and try to return the value, the stringstream will die at the
end of this function, and the pointer will be pointing to a dead memory.
So, a whatBuffer() is used which lives beyond this function call, and points to the right memory
location, as given by what().*/
const char* ANJException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ANJException::GetType() const noexcept
{
	return "ANJ Exception";
}

int ANJException::GetLine() const noexcept
{
	return line;
}

const std::string& ANJException::GetFile() const noexcept
{
	return file;
}

std::string ANJException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	//ostringstream is used to format all this text stuff
	oss << " [File]->" << file << std::endl << "[Line]->" << line;
	return oss.str();
}

