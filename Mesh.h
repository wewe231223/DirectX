#pragma once
class Mesh{
public:
	Mesh();
	~Mesh();

private:
	std::string m_sName{};
	ComPtr<ID3DBlob> m_d3dVertexBufferOnCPU{ nullptr };
	ComPtr<ID3DBlob> m_d3dIndexBufferOnCPU{ nullptr };

	ComPtr<ID3D12Resource> m_d3dVertexBufferOnGPU{ nullptr };
	ComPtr<ID3D12Resource> m_d3dIndexBufferOnGPU{ nullptr };

	ComPtr<ID3D12Resource> m_d3dVertexBufferUploader{ nullptr };
	ComPtr<ID3D12Resource> m_d3dIndexBufferUploader{ nullptr };

	UINT m_nVertexByteStride{ 0 };
	UINT m_nVertexByteSize{ 0 };
	UINT m_nIndexBufferByteSize{ 0 };
	DXGI_FORMAT m_dxgiIndexFormat{ DXGI_FORMAT_R16_UINT };


};

