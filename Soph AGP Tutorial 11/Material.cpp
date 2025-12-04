#include "Material.h"

#include "Renderer.h"
#include "Texture.h"
#include "ShaderLoading.h"
#include <d3d11.h>

Material::Material(std::string name, Renderer& renderer,
	std::string vShaderFilename, std::string fShaderFilename,
	Texture* texture) 
	: name(name), renderer(renderer, dev(renderer.GetDevice()), devCon(renderer.GetDeviceCon()), texture(texture) {

}