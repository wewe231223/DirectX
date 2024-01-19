#include "pch.h"
#include "Camera.h"
#include "Input.h"
#include "Math.h"

Camera::Camera(){

}

Camera::Camera(const ApplicationUtil::WindowInfo* MainWindowInfo,DirectX::XMVECTOR vEye, DirectX::XMVECTOR vAt, float fPov, float fNearZ,float fFarZ)
	: m_vEye(vEye), m_vAt(vAt), m_fPov(fPov), m_fNearZ(fNearZ), m_fFarZ(fFarZ), m_pMainWindowInfo(MainWindowInfo){

	m_fAspect = static_cast<float>(m_pMainWindowInfo->Width) / static_cast<float>(m_pMainWindowInfo->Height);
	

	DirectX::XMFLOAT4X4 ViewMatrix{};
	DirectX::XMFLOAT4X4 ProjectionMatrix{};

	DirectX::XMMATRIX Proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fPov), m_fAspect, m_fNearZ, m_fFarZ);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(m_vEye, m_vAt, m_vUp);

	DirectX::XMStoreFloat4x4(&m_cameraData.ViewProjection,DirectX::XMMatrixTranspose( View * Proj) );
	
	m_vBasisZ = DirectX::XMVector3Normalize(DirectX::XMVectorNegate(m_vAt));
	m_vBasisX = DirectX::XMVector3Normalize(DirectX::XMVector3Dot(m_vUp, m_vBasisZ));
	m_vBasisY = DirectX::XMVector3Normalize(DirectX::XMVector3Dot(m_vBasisZ, m_vBasisX));

	OutputDebugString(std::to_wstring(m_fAspect).c_str());
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
	LONG DeltaMouseX = INPUT->GetDeltaMouseX();
	LONG DeltaMouseY = INPUT->GetDeltaMouseY();

	if (DeltaMouseX != 0 and DeltaMouseY != 0) {
		
		DirectX::XMMATRIX XRotate =  DirectX::XMMatrixRotationAxis(m_vBasisX, Vector::Math::Radians(static_cast<float>(-DeltaMouseX)));
		DirectX::XMMATRIX YRotate = DirectX::XMMatrixRotationAxis(m_vBasisY, Vector::Math::Radians(static_cast<float>(-DeltaMouseY)));

		m_vAt = DirectX::XMVector3Transform(m_vAt, XRotate);
		m_vAt = DirectX::XMVector3Transform(m_vAt, YRotate);

		

		m_vBasisZ = DirectX::XMVector3Normalize(DirectX::XMVectorNegate(m_vAt));
		m_vBasisX = DirectX::XMVector3Normalize(DirectX::XMVector3Dot(m_vUp, m_vBasisZ));
		m_vBasisY = DirectX::XMVector3Normalize(DirectX::XMVector3Dot(m_vBasisZ, m_vBasisX));

	}

	
}
