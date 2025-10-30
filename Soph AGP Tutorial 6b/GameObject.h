#pragma once

#include <string>

#include "Transform.h"

class Mesh;

class GameObject
{
private:
	std::string name = "GameObject";
public:
	Transform transform;
	Mesh* mesh;

	std::string GetName() { return name; }
	GameObject(std::string objectName, Mesh* objectMesh);
};

