#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h> 


class Window
{
	const LPCWSTR CLASS_NAME = L"StortSpel_CLASS_INTE_HELT_HUNDRA_PA_VAD_DENNA_GOR";
	const LPCWSTR projectTitel = L"Once upon a time in Katrineholm";

public:
	Window(HINSTANCE hInstance);
	virtual ~Window();

	/// <summary>Opens the win32 window.</summary>
	void Open(size_t width, size_t height);
	size_t GetWidth() const { return this->width; }
	size_t GetHeight() const { return this->height; }
	float GetWindowAspect() const { return static_cast<float>(this->width) / static_cast<float>(this->height); }

	HWND GetHWND() const { return this->hwnd; }
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE hInstance;
	size_t width, height;
	HWND hwnd;
};