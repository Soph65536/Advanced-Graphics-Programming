#pragma once

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;

class Window;

class Renderer
{
private:
	IDXGISwapChain* swapChain = nullptr; //pointer to swap chain reference
	ID3D11Device* dev = nullptr; //pointer to direct3d device interface
	ID3D11DeviceContext* devCon = nullptr; //pointer to direct3d device context

	Window& window;
public:
	Renderer(Window& inWindow);
	void Clean();
};

