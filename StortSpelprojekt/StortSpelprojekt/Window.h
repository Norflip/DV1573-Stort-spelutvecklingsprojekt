#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h> 
#include <fcntl.h>
#include <io.h>
#include <iostream>



class Window
{
	const LPCWSTR CLASS_NAME = L"StortSpel_CLASS";
	const LPCWSTR projectTitel = L"Test Window";

public:
	Window(HINSTANCE hInstance);
	virtual ~Window();

	void Open(size_t width, size_t height);
	size_t GetWidth() const { return this->width; }
	size_t GetHeight() const { return this->height; }

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE hInstance;
	size_t width, height;
	HWND hwnd;
};