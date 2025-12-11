#pragma once
#include <d3d11.h>
#include <vector>
#include <SpriteFont.h>

#include "Transform.h"
#include "Camera.h"
#include "Lighting.h"

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11BlendState;

class Window;
class GameObject;

class Renderer
{
private:
	Window& window;

	IDXGISwapChain* swapChain = nullptr; //pointer to swap chain reference
	ID3D11Device* dev = nullptr; //pointer to direct3d device interface
	ID3D11DeviceContext* devCon = nullptr; //pointer to direct3d device context
	
	ID3D11RenderTargetView* backBuffer = nullptr; //a buffer that can be used to render to
	ID3D11DepthStencilView* depthBuffer = nullptr; //the pointer to our depth buffer
	ID3D11DepthStencilState* depthWriteOff = nullptr;

	ID3D11Buffer* cBuffer_PerObject = nullptr; //constant buffer object
	ID3D11Buffer* cBuffer_PerFrame = nullptr; //constant buffer frame
	//ID3D11Buffer* cBuffer_Lighting = nullptr; //constant buffer lighting

	ID3D11RasterizerState* rasterizerCullBack = nullptr;
	ID3D11RasterizerState* rasterizerCullFront = nullptr;
	ID3D11RasterizerState* rasterizerCullNone = nullptr;

	ID3D11BlendState* blendOpaque = nullptr;
	ID3D11BlendState* blendTransparent = nullptr;

	DirectX::SpriteFont* font = nullptr;
	DirectX::SpriteBatch* spriteBatch = nullptr;

	long InitD3D();
	long InitDepthBuffer();
	void InitGraphics();
	void InitDepthStencilState();
	void InitRasterizers();
	void InitBlendModes();
	void DrawSkybox();
	void RenderText(const char* text, int x, int y);
public:
	ID3D11Device* GetDevice() { return dev; }
	ID3D11DeviceContext* GetDeviceCon() { return devCon; }

	Renderer(Window& inWindow);
	void RenderFrame();
	void Clean();

	Camera camera;
	GameObject* skyboxObject = nullptr;

	//std::vector is a list!!! so we are storing a list of all our gameobjects
	std::vector<GameObject*> gameObjects;
	void RegisterGameObject(GameObject* e);
	void RemoveGameObject(GameObject* e);

	DirectX::XMVECTOR ambientLightColour{ 0.1f, 0.1f, 0.1f };
	DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
	PointLight pointLights[MAX_POINT_LIGHTS];
};

