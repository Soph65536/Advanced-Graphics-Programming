#include "Material.h"

#include <d3d11.h>

#include "Renderer.h"
#include "Texture.h"
#include "ShaderLoading.h"
#include "Debug.h"

Material::Material(std::string name, Renderer& renderer,
	std::string vShaderFilename, std::string fShaderFilename,
	Texture* texture) 
	: name(name), renderer(renderer), dev(renderer.GetDevice()), devCon(renderer.GetDeviceCon()), texture(texture) {
	
	HRESULT hr;

	hr = ShaderLoading::LoadVertexShader(vShaderFilename, dev, &vShader, &vLayout);
	if (FAILED(hr)) {
		LOG("Material " + name + " failed to load vertex shader, aborting.");
		return;
	}

	hr = ShaderLoading::LoadFragmentShader(fShaderFilename, dev, &fShader);
	if (FAILED(hr)) {
		LOG("Material " + name + " failed to load fragment shader, aborting.");
		return;
	}
}

Material::~Material() {
	return;
}

void Material::Bind() {
	devCon->VSSetShader(vShader, 0, 0);
	devCon->PSSetShader(fShader, 0, 0);
	devCon->IASetInputLayout(vLayout);

	if (texture != nullptr) {
		ID3D11SamplerState* s = texture->GetSampler();
		devCon->PSSetSamplers(0, 1, &s);
		ID3D11ShaderResourceView* t = texture->GetTexture();
		devCon->PSSetShaderResources(0, 1, &t);
	}

	if (cBuffer != nullptr) {
		devCon->VSSetConstantBuffers(0, 1, &cBuffer);
	}
}

void Material::CreateCBuffer(unsigned int byteWidth, ID3D11Buffer * &targetBuffer) {
	D3D11_BUFFER_DESC cbd{ 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = byteWidth;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(dev->CreateBuffer(&cbd, NULL, &targetBuffer))) {
		LOG("Failed to create cbuffer for material " + name);
	}
}

void Material::UpdateCBuffer(CBufferBase& cbData, ID3D11Buffer*& targetBuffer) {
	devCon->UpdateSubresource(targetBuffer, 0, 0, &cbData, 0, 0);
}