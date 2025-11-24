#pragma once
#include <d3d11.h>
#include <vector>
#include <SpriteFont.h>

#include "Transform.h"
#include "Camera.h"

#define MAX_POINT_LIGHTS 8

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11BlendState;

struct PointLight {
	DirectX::XMVECTOR position{ 0, 0, 0 };
	DirectX::XMVECTOR colour{ 1, 1, 1 };

	float strength = 10;
	bool enabled = false; //disabled by default so we dont accidentally use any uninitialised lights
	float padding[2]; //exists to bulk the struct size to a multiple of 16 bytes
};

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
	ID3D11DepthStencilState* depthWriteOff = nullptr;

	ID3D11RasterizerState* rasterizerCullBack = nullptr;
	ID3D11RasterizerState* rasterizerCullNone = nullptr;

	ID3D11BlendState* blendOpaque = nullptr;
	ID3D11BlendState* blendTransparent = nullptr;

	DirectX::XMVECTOR ambientLightColour{ 0.1f, 0.1f, 0.1f };
	DirectX::XMVECTOR directionalLightColour{ 0, 0.8f, 0.75f };
	DirectX::XMVECTOR directionalLightSourcePos{ 0.3f, 0.7f, 0.7f };

	DirectX::SpriteFont* font = nullptr;
	DirectX::SpriteBatch* spriteBatch = nullptr;

	long InitD3D();

	ID3D11VertexShader* pVS = nullptr;
	ID3D11PixelShader* pFS = nullptr;
	ID3D11InputLayout* pIL = nullptr;
	ID3D11Buffer* vBuffer = nullptr; //vertex buffer
	ID3D11Buffer* iBuffer = nullptr; //index buffer
	ID3D11Buffer* cBuffer_PerObject = nullptr; //constant buffer

	long InitDepthBuffer();
	long InitPipeline();
	void InitGraphics();
	void InitDepthStencilState();
	void InitRasterizers();
	void InitBlendModes();
	void RenderText(const char* text, int x, int y);

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

	PointLight pointLights[MAX_POINT_LIGHTS];

	Camera camera;
};

