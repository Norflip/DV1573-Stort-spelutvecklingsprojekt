#include "Input.h"
#include <iostream>
Input::Input()
{
	/*height = width = 0;
	currentMouse = mouse.GetState();
	previousMouse = mouse.GetState();

	currentKey = keyboard.GetState();
	previousKey = keyboard.GetState();*/
	//WWND UNINITIALIZED
}

Input::Input(HWND window, size_t width, size_t height) : hwnd(window), height(height), width(width)
{
	mouse.SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	currentMouse = mouse.GetState();
	previousMouse = mouse.GetState();

	currentKey = keyboard.GetState();
	previousKey = keyboard.GetState();

};
bool Input::GetKey(DirectX::Keyboard::Keys key) const
{
	return currentKey.IsKeyDown(key);
}

bool Input::GetKeyDown(DirectX::Keyboard::Keys key) const
{
	return currentKey.IsKeyDown(key) && !previousKey.IsKeyDown(key);
}

bool Input::GetKeyUp(DirectX::Keyboard::Keys key) const
{
	return !currentKey.IsKeyDown(key) && previousKey.IsKeyDown(key);
}

void Input::setMouseMode(DirectX::Mouse::Mode)
{
}



bool Input::GetLeftMouseKey() const
{
	return  currentMouse.leftButton;
}

bool Input::GetLeftMouseKeyDown() const
{

	return currentMouse.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED;
}

bool Input::GetLeftMouseKeyUp() const
{
	return false;
}

bool Input::GetRightMouseKey() const
{
	return false;
}

bool Input::GetRightMouseKeyDown() const
{
	return false;
}

bool Input::GetRightMouseKeyUp() const
{
	return false;
}

POINTS Input::GetMousePos() const
{
	return POINTS();
}

Input::~Input()
{
}

void Input::updateInputs()
{
	previousMouse = currentMouse;
	currentMouse = mouse.GetState();
	
	previousKey = currentKey;
	currentKey = keyboard.GetState();
	keyboardButtons.Update(keyboard.GetState());

	if (keyboardButtons.IsKeyPressed(DirectX::Keyboard::Q))
	{
		std::cout << "test";
	}
	if (keyboardButtons.IsKeyReleased(DirectX::Keyboard::Q))
	{
		std::cout << "test2";
	}
	if (keyboard.GetState().Q)
	{
		std::cout << "ballahir";
	}
}

void Input::updateMsg(UINT umsg, WPARAM wParam, LPARAM lParam)
{
	DirectX::Keyboard::ProcessMessage(umsg, wParam, lParam);
	DirectX::Mouse::ProcessMessage(umsg, wParam, lParam);
}
