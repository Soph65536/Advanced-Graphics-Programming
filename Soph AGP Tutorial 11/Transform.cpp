#include "Transform.h"
#include <DirectXMath.h>

DirectX::XMMATRIX Transform::GetWorldMatrix() {
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScalingFromVector(scale);
	DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYawFromVector(rotation);
	DirectX::XMMATRIX translationMat = DirectX::XMMatrixTranslationFromVector(position);
	return scaleMat * rotationMat * translationMat;
}

DirectX::XMVECTOR Transform::GetPosition() {
	return position;
}

DirectX::XMVECTOR Transform::GetRotation() {
	return rotation;
}

DirectX::XMVECTOR Transform::GetScale() {
	return scale;
}

DirectX::XMVECTOR Transform::GetMin() {
	return DirectX::XMVectorAdd(position, DirectX::XMVectorScale(scale, -0.5)); //subtract half of scale from position
}

DirectX::XMVECTOR Transform::GetMax() {
	return DirectX::XMVectorAdd(position, DirectX::XMVectorScale(scale, 0.5)); //add half of sacle to position
}

DirectX::XMVECTOR Transform::GetForward() {
	float pitch = DirectX::XMVectorGetX(rotation);
	float yaw = DirectX::XMVectorGetY(rotation);

	DirectX::XMVECTOR direction{
		cosf(pitch) * sinf(yaw), //x
		sinf(pitch),			 //y
		cosf(pitch) * cosf(yaw), //z
		0.0f
	};

	return DirectX::XMVector3Normalize(direction);
}

DirectX::XMVECTOR Transform::GetRight() {
	float pitch = DirectX::XMVectorGetX(rotation);
	float yaw = DirectX::XMVectorGetY(rotation);
	float roll = DirectX::XMVectorGetZ(rotation);

	DirectX::XMVECTOR direction{
		cosf(roll) * cosf(yaw) + sinf(roll) * sinf(pitch) * sinf(yaw),	//x
		sinf(roll) * cosf(pitch),										//y
		cosf(roll) * -sinf(yaw) + sinf(roll) * sinf(pitch) * cosf(yaw), //z
		0.0f
	};

	return DirectX::XMVector3Normalize(direction);
}

DirectX::XMVECTOR Transform::GetUp() {
	DirectX::XMVECTOR cross = DirectX::XMVector3Cross(GetForward(), GetRight());

	return DirectX::XMVector3Normalize(cross);
}


void Transform::SetPosition(DirectX::XMVECTOR newPosition) {
	position = newPosition;
}

void Transform::Translate(DirectX::XMVECTOR translation) {
	position = DirectX::XMVectorAdd(position, translation);
}


void Transform::Rotate(DirectX::XMVECTOR inRotation) {
	rotation = DirectX::XMVectorAddAngles(rotation, inRotation);
}


void Transform::SetScale(DirectX::XMVECTOR newScale) {
	scale = newScale;
}