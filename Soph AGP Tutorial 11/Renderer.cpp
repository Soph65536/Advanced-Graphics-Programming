#include "Renderer.h"
#include "Window.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "GameObject.h"
#include "ShaderLoading.h"
#include "ModelLoader.h"
#include "Debug.h"

#include <d3d11.h>
#include <SpriteFont.h>

#include"DirectXMath.h"
using namespace DirectX;

struct CBuffer_PerObject {
	XMMATRIX world;
	XMMATRIX WVP;
};

struct CBuffer_PerFrame {
	XMFLOAT3 cameraPosition;
	float padding;
};

//struct CBuffer_Lighting {
//	DirectX::XMVECTOR ambientLightColour = { 1, 1, 1 };
//	DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
//	PointLight pointLights[MAX_POINT_LIGHTS];
//};

Renderer::Renderer(Window& inWindow)
	: window(inWindow) {

	if (InitD3D() != S_OK) {
		LOG("Failed to initialise D3D renderer");
		return;
	}

	InitGraphics();
	InitDepthStencilState();
	InitRasterizers();
	InitBlendModes();
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

void Renderer::InitGraphics() {
	//create the per object constant buffer
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CBuffer_PerObject);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(dev->CreateBuffer(&cbd, NULL, &cBuffer_PerObject))) {
		LOG("Failed to create constant buffer per object");
		return;
	}

	//create the per frame constant buffer
	cbd.ByteWidth = sizeof(CBuffer_PerFrame);
	if (FAILED(dev->CreateBuffer(&cbd, NULL, &cBuffer_PerFrame))) {
		LOG("Failed to create constant buffer per frame");
		return;
	}

	////create the lighting constant buffer (same settings as above so we just need to change the size)
	//cbd.ByteWidth = sizeof(CBuffer_Lighting);
	//if (FAILED(dev->CreateBuffer(&cbd, NULL, &cBuffer_Lighting))) {
	//	LOG("Failed to create constant buffer lighting");
	//	return;
	//}

	font = new DirectX::SpriteFont(dev, L"Assets/Fonts/myfile.Spritefont");
	spriteBatch = new DirectX::SpriteBatch(devCon);
}

void Renderer::InitDepthStencilState() {
	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	//depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dev->CreateDepthStencilState(&dsDesc, &depthWriteOff);
}

void Renderer::InitRasterizers() {
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	//rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	//create no culling rasterizer
	dev->CreateRasterizerState(&rsDesc, &rasterizerCullNone);
	//create backface culling rasterizer
	rsDesc.CullMode = D3D11_CULL_BACK;
	dev->CreateRasterizerState(&rsDesc, &rasterizerCullBack);
	//create frontface culling rasterizer
	rsDesc.CullMode = D3D11_CULL_FRONT;
	dev->CreateRasterizerState(&rsDesc, &rasterizerCullFront);
}

void Renderer::InitBlendModes() {
	D3D11_BLEND_DESC bdDesc = { 0 };
	bdDesc.IndependentBlendEnable = FALSE;
	bdDesc.AlphaToCoverageEnable = FALSE;
	bdDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bdDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bdDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bdDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bdDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bdDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bdDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bdDesc.RenderTarget[0].BlendEnable = TRUE;
	dev->CreateBlendState(&bdDesc, &blendTransparent);
	bdDesc.RenderTarget[0].BlendEnable = FALSE;
	dev->CreateBlendState(&bdDesc, &blendOpaque);
}

void Renderer::RenderText(const char* text, int x, int y) {
	////get current depth stencil
	//ID3D11DepthStencilState* depthState;
	//devCon->OMGetDepthStencilState(&depthState, 0);

	//draw text
	spriteBatch->Begin();
	font->DrawString(spriteBatch, text, DirectX::XMFLOAT2(x, y));
	spriteBatch->End();

	//restore previous depth stencil
	devCon->OMSetDepthStencilState(nullptr, 0);
}

void Renderer::DrawSkybox() {
	if (skyboxObject == nullptr) return;

	//front face culling and disable depth write
	devCon->OMSetDepthStencilState(depthWriteOff, 1);
	devCon->RSSetState(rasterizerCullFront);

	//constant buffer data (manually rolling this for skybox. usually handled in renderframe)
	CBuffer_PerObject cbuf;
	XMMATRIX translation, projection, view;
	XMVECTOR cameraPosition = camera.transform.GetPosition();
	translation = XMMatrixTranslation(XMVectorGetX(cameraPosition), XMVectorGetY(cameraPosition), XMVectorGetZ(cameraPosition));
	projection = camera.GetProjectionMatrix(window.GetWidth(), window.GetHeight());
	view = camera.GetViewMatrix();
	cbuf.WVP = translation * view * projection;
	devCon->UpdateSubresource(cBuffer_PerObject, 0, 0, &cbuf, 0, 0);
	devCon->VSSetConstantBuffers(12, 1, &cBuffer_PerObject);

	skyboxObject->material->UpdateMaterial(skyboxObject);
	skyboxObject->material->Bind();
	skyboxObject->mesh->Render();

	//back face culling and enable depth write
	devCon->OMSetDepthStencilState(nullptr, 1);
	devCon->RSSetState(rasterizerCullBack);
}

void Renderer::RenderFrame() {
	//clear back buffer with desired colour
	FLOAT bg[4] = { 0.0f, 0.4f, 0.3f, 1.0f };
	devCon->ClearRenderTargetView(backBuffer, bg);
	devCon->ClearDepthStencilView(depthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawSkybox();

	//set per frame buffer values
	CBuffer_PerFrame cBufferPerFrameData = {};
	XMStoreFloat3(&cBufferPerFrameData.cameraPosition, camera.transform.GetPosition());
	devCon->UpdateSubresource(cBuffer_PerFrame, NULL, NULL, &cBufferPerFrameData, NULL, NULL);
	devCon->VSSetConstantBuffers(11, 1, &cBuffer_PerFrame);

	CBuffer_PerObject cBufferPerObjectData = {};

	////////create the transform data stuff
	//////CBuffer_Lighting cBufferLightingData;
	//////cBufferPerObjectData.WVP = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX view = camera.GetViewMatrix();
	DirectX::XMMATRIX projection = camera.GetProjectionMatrix(window.GetWidth(), window.GetHeight());

	for (auto obj : gameObjects) {
		//transform
		XMMATRIX world = obj->transform.GetWorldMatrix();
		cBufferPerObjectData.world = world;
		cBufferPerObjectData.WVP = world * view * projection;

		////////lighting
		////////ambient light
		//////cBufferLightingData.ambientLightColour = ambientLightColour;
		////////directional lights
		//////for (size_t i = 0; i < MAX_DIRECTIONAL_LIGHTS; i++) {
		//////	cBufferLightingData.directionalLights[i].enabled = directionalLights[i].enabled;

		//////	//if disabled then skip to next light
		//////	if (!directionalLights[i].enabled) { continue; }

		//////	cBufferLightingData.directionalLights[i].colour = directionalLights[i].colour;
		//////	XMMATRIX transpose = XMMatrixTranspose(world);
		//////	cBufferLightingData.directionalLights[i].sourcePosition = XMVector3Transform(directionalLights[i].sourcePosition, transpose);
		//////}
		////////point lights
		//////for (size_t i = 0; i < MAX_POINT_LIGHTS; i++) {
		//////	cBufferLightingData.pointLights[i].enabled = pointLights[i].enabled;

		//////	//if disabled then skip to next light
		//////	if (!pointLights[i].enabled) { continue; }

		//////	XMMATRIX inverse = XMMatrixInverse(nullptr, world);

		//////	cBufferLightingData.pointLights[i].position = XMVector3Transform(pointLights[i].position, inverse);
		//////	cBufferLightingData.pointLights[i].colour = pointLights[i].colour;
		//////	cBufferLightingData.pointLights[i].strength = pointLights[i].strength;
		//////}

		devCon->UpdateSubresource(cBuffer_PerObject, NULL, NULL, &cBufferPerObjectData, NULL, NULL);
		devCon->VSSetConstantBuffers(12, 1, &cBuffer_PerObject);

		//////devCon->UpdateSubresource(cBuffer_Lighting, NULL, NULL, &cBufferLightingData, NULL, NULL);
		//////devCon->VSSetConstantBuffers(13, 1, &cBuffer_Lighting);

		devCon->RSSetState(obj->mesh->isDoubleSided ? rasterizerCullNone : rasterizerCullBack);
		devCon->OMSetBlendState(obj->material->GetTexture()->isTransparent ? blendTransparent : blendOpaque, 0, 0xffffffff);
		devCon->OMSetDepthStencilState(obj->material->GetTexture()->isTransparent ? depthWriteOff : nullptr, 1);

		obj->material->UpdateMaterial(obj);
		obj->material->Bind();
		obj->mesh->Render();
	}

	//RenderText("Hello World", 100, 100);

	//flip the back and front buffers around. display on screen
	swapChain->Present(0, 0);
}

void Renderer::Clean() {

	delete font;
	delete spriteBatch;
	if (cBuffer_PerObject) cBuffer_PerObject->Release();
	//if (iBuffer) iBuffer->Release();
	//if (vBuffer) vBuffer->Release();
	if (depthBuffer) depthBuffer->Release();
	if (backBuffer) backBuffer->Release();
	if (swapChain) swapChain->Release();
	if (dev) dev->Release();
	if (devCon) devCon->Release();
}
