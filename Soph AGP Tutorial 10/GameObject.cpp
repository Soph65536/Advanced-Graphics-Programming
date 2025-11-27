#include "GameObject.h"

GameObject::GameObject(std::string objectName, Mesh* objectMesh, Texture* objectTexture)
	: name(objectName), mesh(objectMesh), texture(objectTexture) {

}

GameObject::GameObject(std::string objectName, Mesh* objectMesh, Texture* objectTexture,
	std::string vertexShaderFilePath, std::string fragmentShaderFilePath)
	: name(objectName), mesh(objectMesh), texture(objectTexture), 
	pVSFilePath(vertexShaderFilePath), pFSFilePath(fragmentShaderFilePath) {
	hasSpecialShaders = true;
}