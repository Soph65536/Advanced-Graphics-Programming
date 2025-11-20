#pragma once

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

	std::string GetName() { return name; }
	GameObject(std::string objectName, Mesh* objectMesh, Texture* objectTexture);
};

