#pragma once
#include <string>
#include <Keyboard.h>
#include <Mouse.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
class Input
{
public:
	Input();
	Input(HWND window, size_t width, size_t height);
	virtual ~Input();



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
	DirectX::Mouse::State currentMouse;
	DirectX::Mouse::State previousMouse;

	// keyboard variables
	DirectX::Keyboard keyboard;
	DirectX::Keyboard::State currentKey;
	DirectX::Keyboard::State previousKey;
};