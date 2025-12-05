#pragma once

#include <string>
#include <d3d11.h>

#include "Transform.h"

class Mesh;
class Material;

class GameObject
{
private:
	std::string name = "GameObject";
public:
	Transform transform;
	Mesh* mesh;
	Material* material;

	bool hasSpecialShaders = false;
	ID3D11VertexShader* pVS = nullptr;
	std::string pVSFilePath = "";
	ID3D11PixelShader* pFS = nullptr;
	std::string pFSFilePath = "";
	ID3D11InputLayout* pIL = nullptr;
	std::string pILFilePath = "";

	std::string GetName() { return name; }
	GameObject(std::string objectName, Mesh* objectMesh, Material* objectMaterial);
};

