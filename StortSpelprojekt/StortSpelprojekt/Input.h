#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <Keyboard.h>
#include <Mouse.h>


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

	void SetMouseMode(DirectX::Mouse::Mode mode);
	//Mouse
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
	HWND hwnd;
	size_t height, width;
	// mouse variables
	
	DirectX::Mouse mouse;
	DirectX::Mouse::ButtonStateTracker mouseButtons;
	// keyboard variables
	DirectX::Keyboard keyboard;
	DirectX::Keyboard::KeyboardStateTracker	keyboardButtons;
};