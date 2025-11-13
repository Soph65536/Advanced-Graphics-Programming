#include "BoxCollider.h"

#include "Transform.h"

bool BoxCollider::BoxCollision(Transform obj1, Transform obj2) {
		return DirectX::XMVector4Greater(obj1.GetMax(), obj2.GetMin())
			&& DirectX::XMVector4Less(obj1.GetMin(), obj2.GetMax());
}
