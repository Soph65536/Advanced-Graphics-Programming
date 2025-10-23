#include "Transform.h"
#include <DirectXMath.h>

DirectX::XMMATRIX Transform::GetWorldMatrix() {
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScalingFromVector(scale);
	DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation);
	DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(position);
	return scaleMat * rotationMat * translationMat;
}


void Transform::Translate(DirectX::XMVECTOR translation) {
	position = DirectX::XMVectorAdd(position, translation);
}

void Transform::Translate(float x, float y, float z) {
	DirectX::XMVECTOR translation{ x, y, z };
	position = DirectX::XMVectorAdd(position, translation);
}


void Transform::Rotate(DirectX::XMVECTOR inRotation) {
	rotation = DirectX::XMVectorAddAngles(rotation, inRotation);
}