#include "Window.h"

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

	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASS));
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = WindowProc;
	wndclass.lpszClassName = CLASS_NAME;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	RegisterClass(&wndclass);

	// Resizes window rect
	RECT windowRect = { 50, 50, width, height };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int wWidth = windowRect.right - windowRect.left;
	int wHeight = windowRect.bottom - windowRect.top;
	this->hwnd = CreateWindowExW(0, CLASS_NAME, projectTitel, WS_OVERLAPPEDWINDOW, windowRect.left, windowRect.top, wWidth, wHeight, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, SW_SHOW);
}

LRESULT Window::WindowProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch (umsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

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
