#pragma once
//Header file to incorporate all sort of stuff related to keyboard input handing

/*Proposed model to follow->
Let's look into it like a client-server sort of architecture. The client in
our case is the game logic and the server is Windows. So, our Keyboard class is gonna
have two sets of interfaces-
One faces towards the server side, and is responsible for maintaining
key states like KEYUP,KEYDOWN and KEYCHAR.
The other one faces towards the client side, and is responsible for maintaining the
queue in which keyboard events have occured, like whether or not key is pressed,
or which key is pressed etc, which can be used for input purpose by the game logic.*/

/*------------Class Starts Here---------------*/

#include<queue>
#include<bitset>

class Keyboard
{
	friend class Window;
	/*A friend class is a class that can access the private and protected members of a class 
	in which it is declared as friend. This is needed when we want to allow a particular class 
	to access the private and protected members of a class.*/
public:
	class Event {
	public:
		//enum defines three type of events -> Press, Release and Invalid
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code; //code of the key that's involved in the event
	public:
		Event() noexcept: type(Type::Invalid),code(0u) //Default constructor sets event to invalid type
		{}
		Event(Type type,unsigned char code) noexcept : type(type),code(code)
		{}
		bool IsPress() const noexcept //Check if the event a Press event
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept //Check if the event is a release event
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept //Check if the event is an invalid event
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept //Get the code of the key involved in that event
		{
			return code;
		}
	};
public:
	Keyboard() = default; //Default Keyboard Construction
	Keyboard(const Keyboard&) = delete; //Delete Copy
	Keyboard& operator=(const Keyboard&) = delete; //Delete Copy Assignment

	//Key Event Functions
	bool KeyIsPressed(unsigned char keycode)const noexcept; //Pass in a keycode-> Will tell if key is currently being pressed
	Event ReadKey() noexcept;	//Pull an event from off the event queue
	bool KeyIsEmpty() const noexcept;	//Will check if there is any event key in event queue
	void FlushKey() noexcept;	//Will flush that queue

	//Char event functions
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept; //This FLUSH will flush both the key and char queues

	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool AutoRepeatIsEnabled() const noexcept;

private:
	//The following functions are used by window class.
	//How? By making window a friend of keyboard class.
	//Since they are not the part of public interface, client can't use 'em.
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept; //Clears the bitset(defined in next private block), which holds all the key state
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
	/*The default buffer size defined below is 16u. If the buffer size exceeds that, TrimBuffer removes
	additional stuff from the buffer until the Buffer's size is down to 16 again.*/
private:
	static constexpr unsigned int nKeys = 256u; //We have set this size because this is the maximum amount of keycode we can ever have.
	static constexpr unsigned int bufferSize = 16u;
	bool AutoRepeatEnabled = false;
	std::bitset<nKeys> KeyStates; //It packs all 256 bools into a single bit and allows us to access them by index
	std::queue<Event> KeyBuffer; //Standard FIFO structure
	std::queue<char> CharBuffer;

};