#include "Renderer.h"
#include "Window.h"
#include "ShaderLoading.h"
#include "Debug.h"

#include <d3d11.h>

#include"DirectXMath.h"
using namespace DirectX;

struct Vertex {
	XMFLOAT3 Pos;
	XMFLOAT4 Colour;
};

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
	Vertex vertices[] = {
		// x      y      z               r      g      b      a
		{XMFLOAT3{-0.5f, -0.5f, -0.5f}, XMFLOAT4{1.0f,  0.0f,  0.0f,  1.0f}},  // Front BL
		{XMFLOAT3{-0.5f,  0.5f, -0.5f}, XMFLOAT4{0.0f,  1.0f,  0.0f,  1.0f}},  // Front TL
		{XMFLOAT3{ 0.5f,  0.5f, -0.5f}, XMFLOAT4{0.0f,  0.0f,  1.0f,  1.0f}},  // Front TR
		{XMFLOAT3{ 0.5f, -0.5f, -0.5f}, XMFLOAT4{1.0f,  1.0f,  1.0f,  1.0f}},  // Front BR

		{XMFLOAT3{-0.5f, -0.5f,  0.5f}, XMFLOAT4{0.0f,  1.0f,  1.0f,  1.0f}},  // Back BL
		{XMFLOAT3{-0.5f,  0.5f,  0.5f}, XMFLOAT4{1.0f,  0.0f,  1.0f,  1.0f}},  // Back TL
		{XMFLOAT3{ 0.5f,  0.5f,  0.5f}, XMFLOAT4{1.0f,  1.0f,  0.0f,  1.0f}},  // Back TR
		{XMFLOAT3{ 0.5f, -0.5f,  0.5f}, XMFLOAT4{0.0f,  0.0f,  0.0f,  1.0f}},  // Back BR

	};

	//create the vertex buffer
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.Usage = D3D11_USAGE_DYNAMIC; //dynamic allows cpu write and gpu read
	vbd.ByteWidth = sizeof(vertices); //size of buffer - size of vertex * num of vertices
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; //use as vertex buffer
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //allow CPU to write in buffer
	//create vertex buffer
	if (FAILED(dev->CreateBuffer(&vbd, NULL, &vBuffer))) {
		LOG("Failed to create vertex buffer");
		return;
	}

	//copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devCon->Map(vBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); //map the buffer
	memcpy(ms.pData, vertices, sizeof(vertices)); //copy the data into the buffer
	devCon->Unmap(vBuffer, NULL);


	unsigned int indices[] = {/*front*/ 0,1,2,2,3,0, /*back*/ 7,6,5,5,4,7, /*left*/ 4,5,1,1,0,4,
		/*right*/ 3,2,6,6,7,3, /*top*/ 1,5,6,6,2,1, /*bottom*/ 4,0,3,3,7,4 };

	//create the index buffer
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//define the resource data
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = indices;

	//create the index buffer with the device
	if (FAILED(dev->CreateBuffer(&ibd, &initData, &iBuffer))) {
		LOG("Failed to create index buffer");
		return;
	}


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

	//select which vertex buffer to use
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	devCon->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	//select index buffer
	devCon->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

	//select which primitive we are using
	devCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//create the transform data stuff
	CBuffer_PerObject cBufferData;
	cBufferData.WVP = DirectX::XMMatrixIdentity();

	//data for transform1
	DirectX::XMMATRIX world = transform1.GetWorldMatrix();
	DirectX::XMMATRIX view = camera.GetViewMatrix();
	DirectX::XMMATRIX projection = camera.GetProjectionMatrix(window.GetWidth(), window.GetHeight());
	cBufferData.WVP = world * view * projection;
	devCon->UpdateSubresource(cBuffer_PerObject, NULL, NULL, &cBufferData, NULL, NULL);
	devCon->VSSetConstantBuffers(0, 1, &cBuffer_PerObject);

	devCon->DrawIndexed(36, 0, 0);

	//data for transform2
	//we don't need to get view and projection again since it's from the camera
	cBufferData.WVP = transform2.GetWorldMatrix() * view * projection;
	devCon->UpdateSubresource(cBuffer_PerObject, NULL, NULL, &cBufferData, NULL, NULL);
	devCon->DrawIndexed(36, 0, 0);

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
