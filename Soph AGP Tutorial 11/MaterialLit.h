#pragma once
#include "Material.h"
#include "Lighting.h"

#include <DirectXMath.h>

class MaterialLit : public Material
{
protected:
	Texture* skyboxTexture = nullptr;
	ID3D11Buffer* cBufferFragmentShader = nullptr;

	struct DirectionaLightCBData {
		DirectX::XMVECTOR transposedDirection;
		DirectX::XMVECTOR colour;
		unsigned int enabled = 0;
		DirectX::XMFLOAT3 padding;
	};

	struct PointLightCBData {
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR colour;
		float strength = 1;
		unsigned int enabled = 0;
		DirectX::XMFLOAT2 padding;
	};

	struct CBufferLighting : CBufferBase {
		DirectX::XMVECTOR ambientLightColour = { 1, 1, 1 };
		DirectionaLightCBData directionalLights[MAX_DIRECTIONAL_LIGHTS];
		PointLightCBData pointLights[MAX_POINT_LIGHTS];
	};

	struct CBufferFS : CBufferBase {
		float reflectiveness;
		DirectX::XMFLOAT3 padding;
	};

public:
	float reflectiveness = 0.1f;

	MaterialLit(std::string name, Renderer& renderer,
		std::string vShaderFilename, std::string fShaderFilename,
		Texture* texture);

	virtual void Bind() override;
	virtual void UpdateMaterial(GameObject* entity) override;

	void SetReflectionTexture(Texture* reflectionTexture) { skyboxTexture = reflectionTexture; }
};

