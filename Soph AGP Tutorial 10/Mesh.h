#pragma once
#include <string>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

class Renderer;

class Mesh
{
private:
	ID3D11Device* dev;
	ID3D11DeviceContext* devCon;
	ID3D11Buffer* vBuffer = NULL; //vertex buffer
	ID3D11Buffer* iBuffer = NULL; //index buffer

	unsigned int indexCount = 0;

public:
	bool isDoubleSided = false;

	Mesh(Renderer& renderer, std::string objPath, bool doubleSided = false);
	//^ doublesided is an optional parameter that will automatically be set to false if not specified
	void Render();
};

