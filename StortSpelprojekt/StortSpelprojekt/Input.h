#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <Keyboard.h>
#include <Mouse.h>


#define KEY(T) DirectX::Keyboard::T
#define KEY_DOWN(T) Input::Instance().GetKeyDown(DirectX::Keyboard::T)
#define KEY_UP(T) Input::Instance().GetKeyUp(DirectX::Keyboard::T)
#define KEY_PRESSED(T) Input::Instance().GetKey(DirectX::Keyboard::T)

#define LMOUSE_DOWN Input::Instance().GetLeftMouseKeyDown()
#define LMOUSE_UP Input::Instance().GetLeftMouseKeyUp()
#define LMOUSE_PRESSED Input::Instance().GetLeftMouseKey()

#define RMOUSE_DOWN Input::Instance().GetRightMouseKeyDown()
#define RMOUSE_UP Input::Instance().GetRightMouseKeyUp()
#define RMOUSE_PRESSED Input::Instance().GetRightMouseKey()

class Input
{
public:
	Input();
	Input(HWND window, size_t width, size_t height);
	virtual ~Input();

	//Keyboard
	bool GetKey(DirectX::Keyboard::Keys key) const;
	bool GetKeyDown(DirectX::Keyboard::Keys key) const;
	bool GetKeyUp(DirectX::Keyboard::Keys key) const;


	//Mouse
	void SetMouseMode(DirectX::Mouse::Mode mode);
	void SetWindow(HWND hwnd, size_t height, size_t width);
	void ConfineMouse();
	void FreeMouse();


	bool GetLeftMouseKey() const;
	bool GetLeftMouseKeyDown() const;
	bool GetLeftMouseKeyUp() const;

	bool GetRightMouseKey() const;
	bool GetRightMouseKeyDown() const;
	bool GetRightMouseKeyUp() const;

	POINT GetMousePos() const;

	void UpdateInputs();
	void UpdateMsg(UINT umsg, WPARAM wParam, LPARAM lParam);
	
	static Input& Instance() // singleton
	{
		static Input instance;
		return instance;
	}

	Input(Input const&) = delete;
	void operator=(Input const&) = delete;

private:
	size_t height, width;
	RECT windowRect; //confine mouse
	// mouse variables	
	DirectX::Mouse mouse;
	DirectX::Mouse::ButtonStateTracker mouseButtons;
	// keyboard variables
	DirectX::Keyboard keyboard;
	DirectX::Keyboard::KeyboardStateTracker	keyboardButtons;
};

//CHEAT SHEET

/*
if (input.GetKeyDown(KEY(Q))) //Created reference of input in 
	std::cout << "DOWNKEY\t";
if (KEY_DOWN(Q))
{
	std::cout << "DOWNKEY2";
	std::cout << std::endl << input.GetMousePos().x;
	std::cout << std::endl << input.GetMousePos().y;
}
if(Input::Instance().GetKeyDown(DirectX::Keyboard::T))
	std::cout << "DOWNKEY3 ";
if (LMOUSE_DOWN)
	std::cout << "LMOUSEDOWN ";



*/
