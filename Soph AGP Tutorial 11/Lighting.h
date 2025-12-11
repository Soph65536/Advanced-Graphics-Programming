#pragma once
#include <DirectXMath.h>

#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 4

struct DirectionalLight {
	DirectX::XMVECTOR sourcePosition{ 0, 0, 0 };
	DirectX::XMVECTOR colour{ 1, 1, 1 };

	bool enabled = false;
	float padding[3];
};

struct PointLight {
	DirectX::XMVECTOR position{ 0, 0, 0 };
	DirectX::XMVECTOR colour{ 1, 1, 1 };

	float strength = 10;
	bool enabled = false; //disabled by default so we dont accidentally use any uninitialised lights
	float padding[2]; //exists to bulk the struct size to a multiple of 16 bytes
};