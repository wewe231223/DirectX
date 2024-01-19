#pragma once
class Camera{
public:
	Camera();
	Camera(const ApplicationUtil::WindowInfo* MainWindowInfo,DirectX::XMVECTOR vEye, DirectX::XMVECTOR vAt,float fPov = 45.f,float fNearZ = 0.1f,float fFarZ = 1000.f);
	~Camera();
protected:
	struct CameraConstants {
		DirectX::XMFLOAT4X4 ViewProjection;
	};
protected:
	const ApplicationUtil::WindowInfo* m_pMainWindowInfo{ nullptr };

	DirectX::XMVECTOR m_vEye{};
	DirectX::XMVECTOR m_vAt{};
	DirectX::XMVECTOR m_vUp{ 0.f,1.f,0.f };

	float m_fAspect{};
	float m_fPov{};
	float m_fNearZ{};
	float m_fFarZ{};

	DirectX::XMVECTOR m_vBasisX{ 1.f,0.f,0.f };
	DirectX::XMVECTOR m_vBasisY{ 0.f,1.f,0.f };
	DirectX::XMVECTOR m_vBasisZ{ 0.f,0.f,1.f };

	DirectX::XMFLOAT4X4 m_mProjection{};
	DirectX::XMFLOAT4X4 m_mView{};

	std::unique_ptr<ConstantBuffer::RootDescriptorBuffer<CameraConstants>> m_pCameraBuffer{ nullptr };
	CameraConstants m_cameraData{};
public:
	void InitBuffer(ComPtr<ID3D12Device> d3dDevice, UINT nRootSignatureIndex);
	virtual void Render(ComPtr<ID3D12GraphicsCommandList> d3dCommandList);
	virtual void Update(float fDeltaTime);

};

