#include "Transform.h"
#include <DirectXMath.h>

DirectX::XMMATRIX Transform::GetWorldMatrix() {
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScalingFromVector(scale);
	DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation);
	DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(position);
	return scaleMat * rotationMat * translationMat;
}