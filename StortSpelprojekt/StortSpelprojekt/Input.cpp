#include "Input.h"

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



bool Input::GetLeftMouseKey() const
{
	return  currentMouse.leftButton;
}

bool Input::GetLeftMouseKeyDown() const
{

	return currentMouse.leftButton && previousMouse.leftButton;
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
}
