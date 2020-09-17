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

Input::~Input()
{
}
