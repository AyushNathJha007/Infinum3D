#pragma once
#include<queue>

class Mouse {
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			RPress,
			LRelease,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Invalid,
			Enter, //Enter and Leave are added for mouse capture (A case when our cursor leaves the window, but is still trackable).
			Leave
		};
	private:
		Type type;
		bool LeftIsPressed;
		bool RightIsPressed;
		int x;
		int y;
	public:
		Event() noexcept : type(Type::Invalid),LeftIsPressed(false),RightIsPressed(false),x(0),y(0)
		{}
		Event(Type type, const Mouse& parent) noexcept : type(type), LeftIsPressed(parent.LeftIsPressed), RightIsPressed(parent.RightIsPressed),
			x(parent.x), y(parent.y)
		{}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return{ x,y };
		}
		int GetPosX() const noexcept
		{
			return x;
		}
		int GetPosY() const noexcept
		{
			return y;
		}
		bool LeftMBIsPressed() const noexcept
		{
			return LeftIsPressed;
		}
		bool RightMBIsPressed() const noexcept
		{
			return RightIsPressed;
		}
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftMBIsPressed() const noexcept;
	bool RightMBIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	void Flush() noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnLeftMBPressed(int x, int y) noexcept;
	void OnRightMBPressed(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftMBReleased(int x, int y) noexcept;
	void OnRightMBReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void TrimBuffer() noexcept;
	void OnWheelData(int x, int y, int delta) noexcept;
private:
	static constexpr unsigned int bufferSize = 16u;
	//x and y coordinates of the cursor->
	int x;
	int y;
	//State of left key and right key->
	bool LeftIsPressed = false;
	bool RightIsPressed = false;
	bool IsCursorInWindow = false;
	int WheelDeltaCarry = 0; //Accumulates the values received when the wheel scrolls. When it reached a particular threshold, then
	//scroll related tasks will be initiated
	std::queue<Event> buffer; //Unlike keyboard, we have only one buffer here, since we don't have anything like WM_CHAR here
};