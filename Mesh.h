#pragma once
/*
* 메쉬에서 상수 버퍼 업데이트를 담당하게 하자 
*/
class Mesh{
public:
	Mesh(const std::string& sName,std::vector<Vertex>& Vertices, std::vector<std::uint16_t>& Indices);
	~Mesh();
private:
	static ComPtr<ID3D12Resource> m_d3dVertexBufferOnGPU;
	static ComPtr<ID3D12Resource> m_d3dIndexBufferOnGPU;

	static ComPtr<ID3D12Resource> m_d3dVertexBufferUploader;
	static ComPtr<ID3D12Resource> m_d3dIndexBufferUploader;

	static UINT m_nVertexByteStride;
	static UINT m_nVertexByteSize;
	static UINT m_nIndexBufferByteSize;
	static DXGI_FORMAT m_dxgiIndexFormat;

	static std::vector<Vertex> m_vertices;
	static std::vector<std::uint16_t> m_indices;

private:
	std::string m_sName{};
	INT m_nStartVertexLocation{ 0 };
	UINT m_nIndexCount{ 0 };
	UINT m_nStartIndexLocation{ 0 };
public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexView() const;
	D3D12_INDEX_BUFFER_VIEW GetIndexView() const;

	void UploadVertexBuffer(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12GraphicsCommandList> d3dCommandList);
	void UploadIndexBuffer(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12GraphicsCommandList> d3dCommandList);
	void ReleaseUploadBuffer();
	

};

