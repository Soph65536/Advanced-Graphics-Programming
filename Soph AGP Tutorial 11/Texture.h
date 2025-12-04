#pragma once
#include <string>

class Renderer;
struct ID3D11ShaderResourceView; //ref to our texture
struct ID3D11SamplerState; //ref to the sampler we're using (point, bilinear, etc)

class Texture
{
private:
	ID3D11ShaderResourceView* texture = nullptr;
	ID3D11SamplerState* sampler = nullptr;

public:
	enum class TextureType {
		Texture2D,
		Cubemap
	};

	bool isTransparent = false;

	ID3D11ShaderResourceView* GetTexture() { return texture; }
	ID3D11SamplerState* GetSampler() { return sampler; }

	Texture(Renderer& renderer, std::string path, bool transparent = false, TextureType type = TextureType::Texture2D); //refs to our renderer and the texture's file path
	~Texture();
};

