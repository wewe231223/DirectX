#include "pch.h"
#include "Scene.h"

Scene::Scene(){
}

Scene::~Scene(){
}

void Scene::Initialize(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12CommandList> d3dCommandList){
	m_d3dDevice = d3dDevice;
	m_d3dCommandList = d3dCommandList;
}

void Scene::CreateShaderResourceDescriptorHeaps(){
	D3D12_DESCRIPTOR_HEAP_DESC ConstantBufferViewHeapDesc{};
	ConstantBufferViewHeapDesc.NumDescriptors = 1;
	ConstantBufferViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ConstantBufferViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ConstantBufferViewHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&ConstantBufferViewHeapDesc, IID_PPV_ARGS(&m_d3dShaderResourceDescriptorHeap)));
}

void Scene::CreateConstantBuffers(){
}

void Scene::CreateRootSignature(){
	
}

void Scene::CreateShaders(){
}

void Scene::CreateInputLayout(){
}

void Scene::CreateGeometry(){
}

void Scene::CreatePipeLineStateObject(){
}

ComPtr<ID3D12PipelineState> Scene::GetPipeLineStateObject() const
{
	return m_d3dPipeLineStateObject;
}
