#include "Camera.h"

DirectX::XMMATRIX Camera::GetViewMatrix() {
	DirectX::XMVECTOR eyePos = transform.GetPosition();
	DirectX::XMVECTOR lookTo = transform.GetForward();
	DirectX::XMVECTOR camUp = transform.GetUp();
	return DirectX::XMMatrixLookToLH(eyePos, lookTo, camUp);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix(int screenWidth, int screenHeight) {
	return DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(fov),
		screenWidth / (float)screenHeight,
		nearClippingPlane,
		farClippingPlane
	);
}