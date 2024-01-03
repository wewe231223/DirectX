#include "pch.h"
#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Initialize(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12CommandList> d3dCommandList)
{
}

void Scene::CreateDescriptorHeaps()
{
}

void Scene::CreateShaders()
{
}

void Scene::CreateInputLayout()
{
}

ComPtr<ID3D12PipelineState> Scene::GetPipeLineStateObject() const
{
	return m_d3dPipeLineStateObject;
}
