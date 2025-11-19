#include "BoxCollider.h"

#include "Transform.h"

bool BoxCollider::BoxCollision(Transform obj1, Transform obj2) {
	return DirectX::XMVectorGetX(obj1.GetMax()) > DirectX::XMVectorGetX(obj2.GetMin())
		&& DirectX::XMVectorGetY(obj1.GetMax()) > DirectX::XMVectorGetY(obj2.GetMin())
		&& DirectX::XMVectorGetZ(obj1.GetMax()) > DirectX::XMVectorGetZ(obj2.GetMin())

		&& DirectX::XMVectorGetX(obj1.GetMin()) < DirectX::XMVectorGetX(obj2.GetMax())
		&& DirectX::XMVectorGetY(obj1.GetMin()) < DirectX::XMVectorGetY(obj2.GetMax())
		&& DirectX::XMVectorGetZ(obj1.GetMin()) < DirectX::XMVectorGetZ(obj2.GetMax());
}
