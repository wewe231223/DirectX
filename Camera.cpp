#include "pch.h"
#include "Camera.h"

Camera::Camera(){

}

Camera::Camera(DirectX::XMVECTOR vEye, DirectX::XMVECTOR vAt, float fPov, float fNearZ,float fFarZ) 
	: m_vEye(vEye), m_vAt(vAt), m_fPov(fPov), m_fNearZ(fNearZ), m_fFarZ(fFarZ){
	ApplicationFunctions::WindowInfo MainWindowInfo{ ApplicationFunctions::GetMainApplicationWindowInfo() };
	m_fAspect = static_cast<float>(MainWindowInfo.Width) / static_cast<float>(MainWindowInfo.Height);
	

	DirectX::XMFLOAT4X4 ViewMatrix{};
	DirectX::XMFLOAT4X4 ProjectionMatrix{};

	DirectX::XMMATRIX Proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fPov), m_fAspect, m_fNearZ, m_fFarZ);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(m_vEye, m_vAt, m_vUp);

	DirectX::XMStoreFloat4x4(&m_cameraData.ViewProjection,DirectX::XMMatrixTranspose( View * Proj) );
}

Camera::~Camera(){
}

void Camera::InitBuffer(ComPtr<ID3D12Device> d3dDevice, UINT nRootSignatureIndex){
	m_pCameraBuffer = std::make_unique<ConstantBuffer::RootDescriptorBuffer<CameraConstants>>(d3dDevice, nRootSignatureIndex);
}

void Camera::Render(ComPtr<ID3D12GraphicsCommandList> d3dCommandList){
	m_pCameraBuffer->BindCommandList(d3dCommandList);
	m_pCameraBuffer->CopyData(m_cameraData);
}

void Camera::Update(float fDeltaTime){
	
}
