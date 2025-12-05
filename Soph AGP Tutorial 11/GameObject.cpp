#include "GameObject.h"

GameObject::GameObject(std::string objectName, Mesh* objectMesh, Material* objectMaterial)
	: name(objectName), mesh(objectMesh), material(objectMaterial) {

}