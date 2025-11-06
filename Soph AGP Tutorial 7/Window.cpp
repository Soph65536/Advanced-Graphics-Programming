#include "Window.h"
#include "Camera.h"
#include "Debug.h"

LRESULT Window::WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY: //if user closes window
		PostQuitMessage(0); //send a quit message to the app
		return 0;

	//if just started app
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
	case WM_INPUT:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;

	//keyboard events
	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
			//fullscreen toggle
		}
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			DestroyWindow(windowHandle); //destroying window is not same as closing app
			//destroying the window will post a WM_DESTROY which will lead to
			//PostQuitMessage(0) being called
		} //intentionally no break
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	//mouse events
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;

	default:
		return DefWindowProc(windowHandle, message, wParam, lParam); //let windows handle everything else with default handling
	}

	return 0;
}

Window::Window(int width, int height, HINSTANCE instanceHandle, int nCmdShow) 
	: instance(instanceHandle), width(width), height(height){ //< this automatically sets attributes to params

	//register window class (properties for windows we want to create)
	WNDCLASSEX wc = {}; //{} sets all values to 0

	//fill in the window class struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc; //reference to our window procedure function, this function will handle window creation or call DefWindowProc()
	wc.hInstance = instanceHandle;
	wc.lpszClassName = L"Window Class 1";
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; //bg colour of the win32 app (not needed for D3D apps)

	//register the window class with the above struct, and error checking
	if (!RegisterClassEx(&wc)) {
		LOG("Failed to register window class");
	}

	//adjust window dimensions so the top window bar isnt taking pixels away from our app
	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	//create the window and use the result as the handle
	handle = CreateWindowEx(NULL,
		L"Window Class 1", //name of our window class
		windowName,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, //window style with no resizing and maximising
		//WS_OVERLAPPEDWINDOW,						 //alternative window style that allows resizing
		100, 100, //x and y positions of window
		wr.right - wr.left, wr.bottom - wr.top, //width and heights of window
		NULL, //no parent window, NULL
		NULL, //no menus, NULL
		instanceHandle,
		NULL); //used with multiple windows, NULL

	//display the window on screen
	if (Exists()) {
		ShowWindow(handle, nCmdShow);
	}
	else {
		LOG("Failed to create a window");
	}

	//set mouse settings
	mouse.SetWindow(handle);
	mouse.SetMode(DirectX::Mouse::MODE_RELATIVE);
	mouse.SetVisible(false);
}



