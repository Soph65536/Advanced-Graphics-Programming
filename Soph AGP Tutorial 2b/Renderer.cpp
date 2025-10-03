#include "Renderer.h"
#include "Window.h"
#include "Debug.h"
#include <d3d11.h>

Renderer::Renderer(Window& inWindow)
	: window(inWindow) {

	if (InitD3D() != S_OK) {
		LOG("Failed to initialise D3D renderer");
		return;
	}
}

long Renderer::InitD3D(){
	//create a struct to hold info about the swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};
	//fil the swap chain description struct
	scd.BufferCount = 1;								//one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //32 bit colour
	scd.BufferDesc.Width = window.GetWidth();				//set the back buffer width
	scd.BufferDesc.Height = window.GetHeight();				//set the back buffer height
	scd.BufferDesc.RefreshRate.Numerator = 60;			//60FPS
	scd.BufferDesc.RefreshRate.Denominator = 1;			//60/1 = 60FPS
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//intended swapchain use
	scd.OutputWindow = window.GetHandle();
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
		&swapChain,
		&dev,
		NULL,									//out param - will be set to chosen feature level
		&devCon);								//pointer to immediate device context

	if (FAILED(hr)) {
		LOG("Failed to create a renderer");
		return hr; //abort
	}

	ID3D11Texture2D* backBufferTexture = nullptr; //get the address of the back buffer
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);
	if (FAILED(hr)) {
		LOG("Failed to get back buffer texture");
		return hr; //abort
	}

	hr = dev->CreateRenderTargetView(backBufferTexture, NULL, &backBuffer);
	backBufferTexture->Release();
	if (FAILED(hr)) {
		LOG("Failed to create back buffer view");
		return hr; //abort
	}

	//set the back buffer as the current render targe
	devCon->OMSetRenderTargets(1, &backBuffer, NULL);

	//define and set the viewport
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = float(window.GetWidth());
	viewport.Height = float(window.GetHeight());
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	devCon->RSSetViewports(1, &viewport);


	return S_OK;
}

void Renderer::RenderFrame() {
	//clear back buffer with desired colour
	FLOAT bg[4] = { 0.5f, 1.0f, 0.0, 1.0f };
	devCon->ClearRenderTargetView(backBuffer, bg);

	//flip the back and front buffers around. display on screen
	swapChain->Present(0, 0);
}

void Renderer::Clean() {
	if (backBuffer) backBuffer->Release();
	if (swapChain) swapChain->Release();
	if (dev) dev->Release();
	if (devCon) devCon->Release();
}
