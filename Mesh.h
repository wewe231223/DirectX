#pragma once
/*
* 메쉬에서 상수 버퍼 업데이트를 담당하게 하자 
*/
class Mesh{
public:
	Mesh();
	~Mesh();

	struct SubMesh {
		UINT IndexCount{ 0 };
		UINT StartIndexPosition{ 0 };
		INT	 StartVertexPosition{ 0 };
	};

private:
	static ComPtr<ID3D12Resource> m_d3dVertexBufferOnGPU;
	static ComPtr<ID3D12Resource> m_d3dIndexBufferOnGPU;

	static ComPtr<ID3D12Resource> m_d3dVertexBufferUploader;
	static ComPtr<ID3D12Resource> m_d3dIndexBufferUploader;
private:
	std::string m_sName{};
	UINT m_nVertexByteStride{ 0 };
	UINT m_nVertexByteSize{ 0 };
	UINT m_nIndexBufferByteSize{ 0 };
	DXGI_FORMAT m_dxgiIndexFormat{ DXGI_FORMAT_R16_UINT };

	
};

