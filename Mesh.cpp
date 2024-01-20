#include "pch.h"
#include "Mesh.h"
// Statics Initialize 
ComPtr<ID3D12Resource> Mesh::m_d3dVertexBufferOnGPU = nullptr;
ComPtr<ID3D12Resource> Mesh::m_d3dIndexBufferOnGPU = nullptr;
ComPtr<ID3D12Resource> Mesh::m_d3dVertexBufferUploader = nullptr;
ComPtr<ID3D12Resource> Mesh::m_d3dIndexBufferUploader = nullptr;
UINT Mesh::m_nVertexByteStride = sizeof(Vertex);
UINT Mesh::m_nVertexByteSize = 0;
UINT Mesh::m_nIndexBufferByteSize = 0;
DXGI_FORMAT Mesh::m_dxgiIndexFormat = DXGI_FORMAT_R16_UINT;
std::vector<Vertex> Mesh::m_vertices = std::vector<Vertex>{};
std::vector<std::uint16_t> Mesh::m_indices = std::vector<uint16_t>{};


Mesh::Mesh(const std::string& sName, std::vector<Vertex>& Vertices, std::vector<std::uint16_t>& Indices){
	m_nStartVertexLocation = static_cast<UINT>(Mesh::m_vertices.size());
	m_nStartIndexLocation = static_cast<UINT>(Mesh::m_indices.size());
	m_nIndexCount = static_cast<UINT>(Indices.size());
	for (const auto& vertex : Vertices) {
		Mesh::m_vertices.emplace_back(vertex);
	}
	for (const auto& index : Indices) {
		Mesh::m_indices.emplace_back(index);
	}
}

Mesh::~Mesh(){
}

void Mesh::UploadVertexBuffer(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12GraphicsCommandList> d3dCommandList){
	if (Mesh::m_d3dVertexBufferOnGPU) Mesh::m_d3dVertexBufferOnGPU.Reset();
	if (Mesh::m_d3dVertexBufferUploader) Mesh::m_d3dVertexBufferUploader.Reset();
	m_d3dVertexBufferOnGPU = ::CreateDefaultBuffer(d3dDevice.Get(), d3dCommandList.Get(), Mesh::m_vertices.data(), static_cast<UINT64>(Mesh::m_vertices.size() * sizeof(Vertex)), Mesh::m_d3dVertexBufferUploader);
	Mesh::m_nVertexByteSize = static_cast<UINT>(Mesh::m_vertices.size() * sizeof(Vertex));
}

void Mesh::UploadIndexBuffer(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12GraphicsCommandList> d3dCommandList) {
	if (Mesh::m_d3dIndexBufferOnGPU) Mesh::m_d3dIndexBufferOnGPU.Reset();
	if (Mesh::m_d3dIndexBufferUploader) Mesh::m_d3dIndexBufferUploader.Reset();
	m_d3dIndexBufferOnGPU = ::CreateDefaultBuffer(d3dDevice.Get(), d3dCommandList.Get(), Mesh::m_indices.data(), static_cast<UINT64>(Mesh::m_indices.size() * sizeof(m_indices)), Mesh::m_d3dIndexBufferUploader);
	Mesh::m_nIndexBufferByteSize = static_cast<UINT>(Mesh::m_indices.size() * sizeof(std::uint16_t));
}

const D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexView() const {
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};
	VertexBufferView.BufferLocation = m_d3dVertexBufferOnGPU->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = m_nVertexByteStride;
	VertexBufferView.SizeInBytes = m_nVertexByteSize;
	return VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexView() const {
	D3D12_INDEX_BUFFER_VIEW IndexBufferView{};
	IndexBufferView.BufferLocation = m_d3dIndexBufferOnGPU->GetGPUVirtualAddress();
	IndexBufferView.Format = m_dxgiIndexFormat;
	IndexBufferView.SizeInBytes = m_nIndexBufferByteSize;
	return IndexBufferView;
}

void Mesh::ReleaseUploadBuffer(){
	if (m_d3dVertexBufferUploader) {
		m_d3dVertexBufferUploader = nullptr;
	}
	if (m_d3dIndexBufferUploader) {
		m_d3dIndexBufferUploader = nullptr;
	}
}



