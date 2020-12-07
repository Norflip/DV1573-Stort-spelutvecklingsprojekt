#include "stdafx.h"
#include "Window.h"
#include "Input.h"

#include <fcntl.h>
#include <io.h>
#include <iostream>

Window::Window(HINSTANCE hInstance) : hInstance(hInstance)
{

}

Window::~Window()
{
	UnregisterClass(CLASS_NAME, hInstance);
	DestroyWindow(hwnd);
}

void Window::Open(size_t width, size_t height)
{
	this->width = width;
	this->height = height;
	//Initializing console

	AllocConsole();
	HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	FILE* fp = _fdopen(hConsole, "w");
	freopen_s(&fp, "CONOUT$", "w", stdout);

	//Creating window
	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASS));
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.lpszClassName = CLASS_NAME;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.hIcon = LoadIcon(hInstance, L"MAINICON");
	RegisterClass(&wndclass);
	RECT windowRect = { 0, 0, (LONG)width, (LONG)height };
	//Adjusting rect to fit the window
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	int wWidth = windowRect.right - windowRect.left;
	int wHeight = windowRect.bottom - windowRect.top;
	this->hwnd = CreateWindowEx(0, CLASS_NAME, projectTitel, WS_OVERLAPPEDWINDOW, windowRect.left, windowRect.top, wWidth, wHeight, nullptr, nullptr, hInstance, nullptr);

	//Moving window
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);		// Window 100 100 before
	ShowWindow(hwnd, SW_SHOW);
	int i = wWidth;
	//SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInstance, L"MAINICON"));
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Window::WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wParam, lParam))
		return true;

	Input::Instance().UpdateMsg(umsg, wParam, lParam);
	switch (umsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			PostMessageA(hwnd, umsg, wParam, lParam);
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(hwnd, &ps);
		}
		return 0;
	}

	return DefWindowProc(hwnd, umsg, wParam, lParam);
}

void Window::Resize(size_t width, size_t height)
{
	this->width = width;
	this->height = height;
	
}
