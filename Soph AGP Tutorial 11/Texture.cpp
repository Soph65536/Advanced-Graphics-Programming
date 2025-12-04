#include "Texture.h"

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <d3d11.h>

#include "Renderer.h"

Texture::Texture(Renderer& renderer, std::string path, bool transparent, TextureType type) 
	: isTransparent(transparent){
	//get device and context from renderer
	ID3D11Device* dev = renderer.GetDevice();
	ID3D11DeviceContext* devCon = renderer.GetDeviceCon();

	//get filepath as wide string then load texture from it
	std::wstring filepath = std::wstring(path.begin(), path.end());

	switch (type) {
	case Texture::TextureType::Texture2D:
		DirectX::CreateWICTextureFromFile(dev, devCon, filepath.c_str(), NULL, &texture);
		break;
	case Texture::TextureType::Cubemap:
		DirectX::CreateDDSTextureFromFile(dev, devCon, filepath.c_str(), NULL, &texture);
		break;
	default:
		break;
	}

	//create sampler description
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC)); //provide correct amount of empty memory for the desc
	//way that we are filtering the sampler
	////samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //bilinear?
	////samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; //anisotropic
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //bilinear
	//wrap, clamp, border, mirror etc options
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//LODs
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//create sampler from desc
	dev->CreateSamplerState(&samplerDesc, &sampler);
}

Texture::~Texture() {
	if (texture) texture->Release();
	if (sampler) sampler->Release();
}