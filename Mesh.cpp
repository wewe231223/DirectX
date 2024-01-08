#include "pch.h"
#include "Mesh.h"

ComPtr<ID3D12Resource> Mesh::m_d3dVertexBufferOnGPU = nullptr;
ComPtr<ID3D12Resource> Mesh::m_d3dIndexBufferOnGPU = nullptr;
ComPtr<ID3D12Resource> Mesh::m_d3dVertexBufferUploader = nullptr;
ComPtr<ID3D12Resource> Mesh::m_d3dIndexBufferUploader = nullptr;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}
