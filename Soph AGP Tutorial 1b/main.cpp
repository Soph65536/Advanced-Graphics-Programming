#include <Windows.h>
#include <d3d11.h>

#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600

//global vars

HINSTANCE gInstanceHandle = NULL; //handle to this instance (our app's location in memory)
HWND gWindowHandle = NULL; //handle to our created window
const wchar_t* windowName = L"DirectX Thing"; //wide char array
//wide character is twice the size of a normal character type, the L"" is used to indicate this is a wide character value

IDXGISwapChain* gSwapChain		= NULL; //pointer to swap chain reference
ID3D11Device* gDev				= NULL; //pointer to direct3d device interface
ID3D11DeviceContext* gDevCon	= NULL; //pointer to direct3d device context

HRESULT InitWindow(HINSTANCE instanceHandle, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND windowH, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT InitD3D(HWND windowH);
void CleanD3D();

//functions

int WINAPI WinMain(_In_ HINSTANCE instanceH, _In_opt_ HINSTANCE prevInstanceH, _In_ LPSTR lpCmdLine, _In_ int  nCmdShow) {
	//initialise window with error check
	if (FAILED(InitWindow(instanceH, nCmdShow))) {
		MessageBox(NULL, L"Failed to create window D:", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	//initialise direct3d with error check
	if (FAILED(InitD3D(gWindowHandle))) {
		MessageBox(NULL, L"Unable to create swapchain and device D:", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	//used to hold windows event messages
	MSG msg;

	//main game loop
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg); //translate certain messages into correct format, namely key presses

			DispatchMessage(&msg); //send the message to the windowproc function

			if (msg.message == WM_QUIT) break; //break out of the loop if get quit message
		}
		else {
			//game code here
		}
	}

	CleanD3D();

	return 0;
}

HRESULT InitWindow(HINSTANCE instanceHandle, int nCmdShow) {
	gInstanceHandle = instanceHandle; //store our instance handle

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
		return E_FAIL;
	}

	//adjust window dimensions so the top window bar isnt taking pixels away from our app
	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	//create the window and use the result as the handle
	gWindowHandle = CreateWindowEx(NULL,
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
	ShowWindow(gWindowHandle, nCmdShow);

	return S_OK;
}

LRESULT CALLBACK WindowProc(HWND windowH, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

	case WM_DESTROY: //if user closes window
		PostQuitMessage(0); //send a quit message to the app
		return 0;

	case WM_KEYDOWN: //if a key is pressed
		switch (wParam) {

		case VK_ESCAPE:
			DestroyWindow(gWindowHandle); //note: destroying the window is not the same as closing the app
			//^ it will post a WM_DESTROY which will lead to PostQuitMessage(0) being called
			break;

		case 'W':
			//w key was pressed
			break;
		}

	default:
		return DefWindowProc(windowH, message, wParam, lParam); //let windows handle everything else with default handling
	}

	return 0;
}

HRESULT InitD3D(HWND windowH) {
	//create a struct to hold info about the swap chain
	DXGI_SWAP_CHAIN_DESC scd = {}; 
	//fil the swap chain description struct
	scd.BufferCount = 1;								//one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //32 bit colour
	scd.BufferDesc.Width = SCREEN_WIDTH;				//set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;				//set the back buffer height
	scd.BufferDesc.RefreshRate.Numerator = 60;			//60FPS
	scd.BufferDesc.RefreshRate.Denominator = 1;			//60/1 = 60FPS
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//intended swapchain use
	scd.OutputWindow = windowH;
	scd.SampleDesc.Count = 1;							//number of samples for AA
	scd.Windowed = TRUE;								//windowed/full screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //allow full screen switching

	HRESULT hr;
	//create a swap chain, device and device context from the scd
	hr = D3D11CreateDeviceAndSwapChain(NULL,	//default graphics adapter
		D3D_DRIVER_TYPE_HARDWARE,				//use hardware acceleration, can also use software or WARP renderers
		NULL,									//used for software driver types
		D3D11_CREATE_DEVICE_DEBUG,				//flags can be OR'd together, we are enabling debug here
		NULL,									//direct3d feature levels. NULL will use d2d11.0 or older
		NULL,									//size of array passed to this ^^^
		D3D11_SDK_VERSION,						//always set to D3D11_SDK_VERSION
		&scd,									//pointer to swap chain description
		&gSwapChain,
		&gDev,
		NULL,									//out param - will be set to chosen feature level
		&gDevCon);								//pointer to immediate device context

	if (FAILED(hr)) return hr;

	return S_OK;
}

void CleanD3D() {
	if (gSwapChain) gSwapChain->Release();
	if (gDev) gDev->Release();
	if (gDevCon) gDevCon->Release();
}