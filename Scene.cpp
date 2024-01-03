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

void Scene::CreateDescriptorHeaps(){
	D3D12_DESCRIPTOR_HEAP_DESC ConstantBufferViewHeapDesc{};
	ConstantBufferViewHeapDesc.NumDescriptors = 1;
	ConstantBufferViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	ConstantBufferViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ConstantBufferViewHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&ConstantBufferViewHeapDesc, IID_PPV_ARGS(&m_d3dConstantBufferViewGHeap)));
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

constexpr std::uint32_t Scene::ConstantBufferBySize(std::uint32_t Byte){
	/*
	* 들어온 크기를 0xff(256 바이트)의 배수로 만들어준다 
	* Byte = 300
	* (300 + 255) = 555
	* 0x022b & ~0x00ff
	* 0x022b & 0xff00
	* 0x0200 = 512 
	*/
	return (Byte + 0xff) & ~0xff;
}

ComPtr<ID3D12PipelineState> Scene::GetPipeLineStateObject() const
{
	return m_d3dPipeLineStateObject;
}
