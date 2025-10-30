#include "Renderer.h"
#include "Window.h"
#include "Mesh.h"
#include "GameObject.h"
#include "ShaderLoading.h"
#include "ModelLoader.h"
#include "Debug.h"

#include <d3d11.h>

#include"DirectXMath.h"
using namespace DirectX;

struct CBuffer_PerObject
{
	XMMATRIX WVP;
};

Renderer::Renderer(Window& inWindow)
	: window(inWindow) {

	if (InitD3D() != S_OK) {
		LOG("Failed to initialise D3D renderer");
		return;
	}

	if (InitPipeline() != S_OK) {
		LOG("Failed to initialise pipeline");
		return;
	}

	InitGraphics();
}

void Renderer::RegisterGameObject(GameObject* e) {
	gameObjects.push_back(e);
	LOG("Registered " + e->GetName() + ".");
}

void Renderer::RemoveGameObject(GameObject* e) {
	auto foundEntity = std::find(gameObjects.begin(), gameObjects.end(), e);
	if (foundEntity != gameObjects.end()) {
		gameObjects.erase(foundEntity);
	}
	//note: will affect index based iterating
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

	hr = InitDepthBuffer();
	if (FAILED(hr)) {
		LOG("Failed to create depth buffer");
		return hr; //abort
	}

	//set the back buffer as the current render target
	devCon->OMSetRenderTargets(1, &backBuffer, depthBuffer);

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

long Renderer::InitDepthBuffer() {
	HRESULT hr;

	//get swap chain settings and assign to scd
	DXGI_SWAP_CHAIN_DESC scd = {};
	swapChain->GetDesc(&scd);

	//z buffer texture description
	D3D11_TEXTURE2D_DESC tex2dDesc = { 0 };
	tex2dDesc.Width = window.GetWidth();
	tex2dDesc.Height = window.GetHeight();
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = scd.SampleDesc.Count; //same sample count as swap chin
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	//z buffer texture
	ID3D11Texture2D* zBufferTexture;
	hr = dev->CreateTexture2D(&tex2dDesc, NULL, &zBufferTexture);
	if (FAILED(hr)) {
		LOG("Failed to create z buffer texture");
		return E_FAIL;
	}

	//create the depth buffer view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC)); //< fill the struct with zeros
	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = dev->CreateDepthStencilView(zBufferTexture, &dsvDesc, &depthBuffer);
	if (FAILED(hr)) {
		LOG("Failed to create depth stencil view");
		return E_FAIL;
	}
	zBufferTexture->Release(); //release zbuffer pointer - we still have depthbuffer, we are only releasing a pointer here

	return S_OK;
}

long Renderer::InitPipeline() {
	ShaderLoading::LoadVertexShader("Compiled Shaders/VertexShader.cso", dev, &pVS, &pIL);
	ShaderLoading::LoadPixelShader("Compiled Shaders/PixelShader.cso", dev, &pPS);

	//set shader objects as active shaders in the pipeline
	devCon->VSSetShader(pVS, 0, 0);
	devCon->PSSetShader(pPS, 0, 0);

	devCon->IASetInputLayout(pIL);

	return S_OK;
}

void Renderer::InitGraphics() {
	//create the constant buffer
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CBuffer_PerObject);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(dev->CreateBuffer(&cbd, NULL, &cBuffer_PerObject))) {
		LOG("Failed to create constant buffer");
		return;
	}
}

void Renderer::RenderFrame() {
	//clear back buffer with desired colour
	FLOAT bg[4] = { 0.0f, 0.4f, 0.3f, 1.0f };
	devCon->ClearRenderTargetView(backBuffer, bg);
	devCon->ClearDepthStencilView(depthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//create the transform data stuff
	CBuffer_PerObject cBufferData;
	cBufferData.WVP = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX view = camera.GetViewMatrix();
	DirectX::XMMATRIX projection = camera.GetProjectionMatrix(window.GetWidth(), window.GetHeight());
	
	for (auto obj : gameObjects) {
		XMMATRIX world = obj->transform.GetWorldMatrix();
		cBufferData.WVP = world * view * projection;
		devCon->UpdateSubresource(cBuffer_PerObject, NULL, NULL, &cBufferData, NULL, NULL);
		devCon->VSSetConstantBuffers(0, 1, &cBuffer_PerObject);
	
		obj->mesh->Render();
	}

	//flip the back and front buffers around. display on screen
	swapChain->Present(0, 0);
}

void Renderer::Clean() {

	if (cBuffer_PerObject) cBuffer_PerObject->Release();
	if (iBuffer) iBuffer->Release();
	if (vBuffer) vBuffer->Release();
	if (depthBuffer) depthBuffer->Release();
	if (backBuffer) backBuffer->Release();
	if (swapChain) swapChain->Release();
	if (dev) dev->Release();
	if (devCon) devCon->Release();
}
