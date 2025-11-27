#pragma once

#include <d3d11.h>
#include <string>

#include "Transform.h"

class Mesh;
class Texture;

class GameObject
{
private:
	std::string name = "GameObject";
public:
	Transform transform;
	Mesh* mesh;
	Texture* texture;

	bool hasSpecialShaders = false;
	ID3D11VertexShader* pVS = nullptr;
	std::string pVSFilePath = "";
	ID3D11PixelShader* pFS = nullptr;
	std::string pFSFilePath = "";
	ID3D11InputLayout* pIL = nullptr;
	std::string pILFilePath = "";

	std::string GetName() { return name; }
	GameObject(std::string objectName, Mesh* objectMesh, Texture* objectTexture);
	GameObject(std::string objectName, Mesh* objectMesh, Texture* objectTexture, 
		std::string vertexShaderFilePath, std::string fragmentShaderFilePath);
};

