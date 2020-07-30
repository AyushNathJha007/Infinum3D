#pragma once
#include<exception>
#include<string>

//ANJException inherits from std::exception
class ANJException :public std::exception
{
public:
	ANJException(int line, const char* file) noexcept;
	const char* what() const noexcept override;	//what() is a virtual function provided by std::exception
	//what() prints out the information about the line from where exception was thrown and the file name
	virtual const char* GetType() const noexcept;	//Gets Exception Type Name
	int GetLine() const noexcept;	//Gets the line where exception occured
	const std::string& GetFile() const noexcept;	//Gets the file
	std::string GetOriginString() const noexcept;	//It formats the exception type, line and the file name all in one single string
private:
	int line;	//Line number from where the exception was thrown
	std::string file;	//File name from where the exception was thrown
protected:
	mutable std::string whatBuffer;
	/*Since what() is declared as const, whatBuffer has to be set mutable
	to set it from within what() function.*/
};