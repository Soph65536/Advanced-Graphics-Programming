#include "GameObject.h"

GameObject::GameObject(std::string objectName, Mesh* objectMesh, Texture* objectTexture)
	: name(objectName), mesh(objectMesh), texture(objectTexture) {

}