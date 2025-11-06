#pragma once
#include "Windows.h"
#include "Keyboard.h"
#include <Mouse.h>

class Camera;

class Window
{
private:
	HWND handle = NULL; //handle to this instance (our app's location in memory)
	HINSTANCE instance = NULL; //handle to our created window
	int width = 32, height = 32;

	DirectX::Keyboard keyboard;
	DirectX::Mouse mouse;

	const wchar_t* windowName = L"DirectX Thing"; //wide char array
	//wide character is twice the size of a normal character type, the L"" is used to indicate this is a wide character value

	static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

public:
	bool Exists() { return handle != NULL; }
	HWND GetHandle() { return handle; }
	int GetWidth() { return width; }
	int GetHeight() { return height; }

	Window(int width, int height, HINSTANCE instanceHandle, int nCmdShow);
};

