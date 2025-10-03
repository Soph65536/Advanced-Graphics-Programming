#pragma once

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;

class Window;

class Renderer
{
private:
	IDXGISwapChain* swapChain = nullptr; //pointer to swap chain reference
	ID3D11Device* dev = nullptr; //pointer to direct3d device interface
	ID3D11DeviceContext* devCon = nullptr; //pointer to direct3d device context
	ID3D11RenderTargetView* backBuffer = nullptr; //a buffer that can be used to render to

	long InitD3D();

	Window& window;
public:
	Renderer(Window& inWindow);
	void RenderFrame();
	void Clean();
};

