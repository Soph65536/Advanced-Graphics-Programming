#pragma once
#include <d3d11.h>
#include <vector>

#include "Transform.h"
#include "Camera.h"

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

class Window;
class GameObject;

class Renderer
{
private:
	IDXGISwapChain* swapChain = nullptr; //pointer to swap chain reference
	ID3D11Device* dev = nullptr; //pointer to direct3d device interface
	ID3D11DeviceContext* devCon = nullptr; //pointer to direct3d device context
	ID3D11RenderTargetView* backBuffer = nullptr; //a buffer that can be used to render to
	ID3D11DepthStencilView* depthBuffer = nullptr; //the pointer to our depth buffer

	long InitD3D();

	ID3D11VertexShader* pVS = nullptr;
	ID3D11PixelShader* pPS = nullptr;
	ID3D11InputLayout* pIL = nullptr;
	ID3D11Buffer* vBuffer = nullptr; //vertex buffer
	ID3D11Buffer* iBuffer = nullptr; //index buffer
	ID3D11Buffer* cBuffer_PerObject = nullptr; //constant buffer

	long InitDepthBuffer();
	long InitPipeline();
	void InitGraphics();

	Window& window;
public:
	ID3D11Device* GetDevice() { return dev; }
	ID3D11DeviceContext* GetDeviceCon() { return devCon; }

	Renderer(Window& inWindow);
	void RenderFrame();
	void Clean();

	//std::vector is a list!!! so we are storing a list of all our gameobjects
	std::vector<GameObject*> gameObjects;
	void RegisterGameObject(GameObject* e);
	void RemoveGameObject(GameObject* e);

	Camera camera;
};

