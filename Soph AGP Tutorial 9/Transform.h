#pragma once

#include <DirectXMath.h>

class Transform
{
private:
	DirectX::XMVECTOR position{ 0, 0, 0 };
	DirectX::XMVECTOR rotation{ 0, 0, 0 };
	DirectX::XMVECTOR scale{ 1, 1, 1 };
public:
	DirectX::XMMATRIX GetWorldMatrix();

	DirectX::XMVECTOR GetPosition();
	DirectX::XMVECTOR GetRotation();
	DirectX::XMVECTOR GetScale();

	DirectX::XMVECTOR GetMin();
	DirectX::XMVECTOR GetMax();

	DirectX::XMVECTOR GetForward();
	DirectX::XMVECTOR GetRight();
	DirectX::XMVECTOR GetUp();

	void SetPosition(DirectX::XMVECTOR newPosition);
	void Translate(DirectX::XMVECTOR translation);

	void Rotate(DirectX::XMVECTOR inRotation);

	void SetScale(DirectX::XMVECTOR newScale);
};

