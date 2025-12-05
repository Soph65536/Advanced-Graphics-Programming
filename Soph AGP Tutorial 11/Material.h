#pragma once
#include <string>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

class Renderer;
class Texture;
class GameObject;

class Material
{
protected:
	struct CBufferBase {

	};

	std::string name;

	Renderer& renderer;
	ID3D11Device* dev = nullptr;
	ID3D11DeviceContext* devCon = nullptr;
	ID3D11VertexShader* vShader = nullptr;
	ID3D11PixelShader* fShader = nullptr;
	ID3D11InputLayout* vLayout = nullptr;

	Texture* texture = nullptr;

	ID3D11Buffer* cBuffer = nullptr;

	void CreateCBuffer(unsigned int byteWidth, ID3D11Buffer*& targetBuffer);
	void UpdateCBuffer(CBufferBase& cbData, ID3D11Buffer*& targetBuffer);
public:
	Material(std::string name, Renderer& renderer,
		std::string vShaderFilename, std::string fShaderFilename,
		Texture* texture);
	~Material();

	Texture* GetTexture() { return texture; }

	virtual void Bind();
	virtual void UpdateMaterial(GameObject* entity) {}
};

