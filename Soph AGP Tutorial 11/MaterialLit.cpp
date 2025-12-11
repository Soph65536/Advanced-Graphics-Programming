#include "MaterialLit.h"

#include <d3d11.h>

#include "Renderer.h"
#include "GameObject.h"
#include "Texture.h"

MaterialLit::MaterialLit(std::string name, Renderer& renderer,
	std::string vShaderFilename, std::string fShaderFilename,
	Texture* texture)
	: Material(name, renderer, vShaderFilename, fShaderFilename, texture) {
	CreateCBuffer(sizeof(CBufferLighting), cBuffer); //slot 0 vs cb
	CreateCBuffer(sizeof(CBufferFS), cBufferFragmentShader); //slot 0 fs cb
}

void MaterialLit::Bind() {
	Material::Bind();

	//vs buffer binding not needed since its done in the base class
	//devCon->VSSetConstantBuffers(0, 1, &cBuffer);

	//bind fragment shader cbuffer
	if (cBufferFragmentShader != nullptr) {
		devCon->PSSetConstantBuffers(0, 1, &cBufferFragmentShader);
	}

	//skybox reflection texture
	if (skyboxTexture != nullptr) {
		ID3D11ShaderResourceView* t = skyboxTexture->GetTexture();
		devCon->PSSetShaderResources(1, 1, &t);
	}
}

void MaterialLit::UpdateMaterial(GameObject* entity) {
	using namespace DirectX;
	Material::UpdateMaterial(entity);

	CBufferLighting cbData;
	//ambient light
	cbData.ambientLightColour = renderer.ambientLightColour;
	
	//directional lights
	DirectionalLight* directionalLights = renderer.directionalLights;
	for (size_t i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i) {
		cbData.directionalLights[i].enabled = directionalLights[i].enabled;

		//if disabled then skip to next light
		if (!directionalLights[i].enabled) { continue; }

		cbData.directionalLights[i].colour = directionalLights[i].colour;
		XMMATRIX transpose = XMMatrixTranspose(entity->transform.GetWorldMatrix()); //transpose rotations
		cbData.directionalLights[i].transposedDirection = XMVector3Transform(XMVector3Normalize(directionalLights[i].sourcePosition), transpose);
	}

	//point lights
	PointLight* pointLights = renderer.pointLights;
	for (size_t i = 0; i < MAX_POINT_LIGHTS; ++i) {
		cbData.pointLights[i].enabled = pointLights[i].enabled;

		//if disabled then skip to next light
		if (!pointLights[i].enabled) { continue; }

		XMMATRIX inverse = XMMatrixInverse(nullptr, entity->transform.GetWorldMatrix());

		cbData.pointLights[i].position = XMVector3Transform(pointLights[i].position, inverse);
		cbData.pointLights[i].colour = pointLights[i].colour;
		cbData.pointLights[i].strength = pointLights[i].strength;
	}

	UpdateCBuffer(cbData, cBuffer);

	//fragment shader
	CBufferFS cbfsData;
	cbfsData.reflectiveness = reflectiveness;
	UpdateCBuffer(cbfsData, cBufferFragmentShader);
}