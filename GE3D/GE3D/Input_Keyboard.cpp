#include "Input_Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept
{
	return KeyStates[keycode]; //To check if key is pressed, we return keystates of keycodes.
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (KeyBuffer.size() > 0u) //Checks if we have any event in the queue
	{
		Keyboard::Event e = KeyBuffer.front();
		KeyBuffer.pop();
		return e;
	}
	else
		return Keyboard::Event(); //As defined, this default constructor returns invalid event
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return KeyBuffer.empty();
}

char Keyboard::ReadChar() noexcept
{
	if (CharBuffer.size() > 0u)
	{
		unsigned char charcode = CharBuffer.front();
		CharBuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return CharBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
	KeyBuffer = std::queue<Event>(); //New empty queue is constructed which is replaces the previous queue
}

void Keyboard::FlushChar() noexcept
{
	CharBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept
{
	AutoRepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
	AutoRepeatEnabled = false;
}

bool Keyboard::AutoRepeatIsEnabled() const noexcept
{
	return AutoRepeatEnabled;
}

//The private side, which our Windows class uses ->

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{

	//Updates both keystate and the keybuffer
	KeyStates[keycode] = true;
	KeyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(KeyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	KeyStates[keycode] = false;
	KeyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(KeyBuffer);
}

void Keyboard::OnChar(char character) noexcept
{
	//There is no state for char, so we just push it on queue
	CharBuffer.push(character);
	TrimBuffer(CharBuffer);
}

void Keyboard::ClearState() noexcept
{
	KeyStates.reset(); //Resets the keystate using an internal reset function
}

//TrimBuffer is a template class. It has been created as a template so that it can work on both queue<char> and queue<event>

template<typename T>

void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}