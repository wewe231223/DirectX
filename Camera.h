#pragma once
class Camera{
public:
	Camera();
	Camera(DirectX::XMVECTOR vEye, DirectX::XMVECTOR vAt);
protected:
	struct CameraConstants {
		DirectX::XMFLOAT4X4 ViewProjection;
	};

	DirectX::XMVECTOR m_vEye{};
	DirectX::XMVECTOR m_vAt{};
	DirectX::XMVECTOR m_vUp{ 0.f,1.f,0.f };

	


};

