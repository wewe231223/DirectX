#include "pch.h"
#include "Scene.h"

Scene::Scene(){
}

Scene::~Scene(){
}

void Scene::Initialize(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12GraphicsCommandList> d3dCommandList){
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
	UINT ElementByteSize = CalcConstantBufferByteSize(sizeof(ObjectConstants));


	CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ConstantResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(ElementByteSize * 1);
	m_d3dDevice->CreateCommittedResource(
		&HeapProperties, 
		D3D12_HEAP_FLAG_NONE, 
		&ConstantResourceDesc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, 
		IID_PPV_ARGS(m_d3dConstantBuffer.GetAddressOf()));

	D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferAdress = m_d3dConstantBuffer->GetGPUVirtualAddress();

	D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc{};
	ConstantBufferViewDesc.BufferLocation = ConstantBufferAdress;
	ConstantBufferViewDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(ObjectConstants));

	m_d3dDevice->CreateConstantBufferView(&ConstantBufferViewDesc, m_d3dShaderResourceDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void Scene::CreateRootSignature(){
	CD3DX12_ROOT_PARAMETER RootParameter[1];

	CD3DX12_DESCRIPTOR_RANGE ConstantBufferDescriptorTable;
	ConstantBufferDescriptorTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,1,0);
	
	RootParameter[0].InitAsDescriptorTable(1, &ConstantBufferDescriptorTable);

	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(1, RootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> SerializedRootSignature{ nullptr };
	ComPtr<ID3DBlob> ErrorBlob{ nullptr };

	ThrowIfFailed(D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, SerializedRootSignature.GetAddressOf(), ErrorBlob.GetAddressOf()));
	ThrowIfFailed(m_d3dDevice->CreateRootSignature(0, SerializedRootSignature->GetBufferPointer(), SerializedRootSignature->GetBufferSize(), IID_PPV_ARGS(m_d3dRootSignature.GetAddressOf())));

	m_d3dCommandList->SetGraphicsRootSignature(m_d3dRootSignature.Get());


}

void Scene::CreateShaders(){
	
	m_d3dVertexShader = ::CompileShader(L"Shader//DefaultShader.hlsl",nullptr, "VertexShader", "vs_5_1");
	m_d3dPixelShader = ::CompileShader(L"Shader//DefaultShader.hlsl", nullptr, "PixelShader", "ps_5_1");

}

void Scene::CreateInputLayout(){
	// 정점별 속성 작성 
	// R,G,..				: 차원의 개수 
	// ex) R32G32B32_FLOAT	: 4차원 
	// 32					: 32비트(12 바이트 : float 당 4바이트 이므로, 3차원 float vector 은 12바이트다) 
	// 0, 12, 24			: OPENGL의 Stride 개념 ( Position, Normal 이 3차원(12 바이트) 이므로 12바이트씩 Stride 증가) 
	m_d3dInputLayout = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,32,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};
}

void Scene::CreateGeometry(){
	std::vector<Vertex> Vertices{};
	Vertices.push_back(Vertex{ DirectX::XMFLOAT3(-1.f,-1.f,-1.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT4(DirectX::Colors::Green) });
	Vertices.push_back(Vertex{ DirectX::XMFLOAT3(-1.f, 1.f,-1.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT4(DirectX::Colors::Green) });
	Vertices.push_back(Vertex{ DirectX::XMFLOAT3( 1.f, 1.f,-1.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT4(DirectX::Colors::Green) });
	Vertices.push_back(Vertex{ DirectX::XMFLOAT3( 1.f,-1.f,-1.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT4(DirectX::Colors::Green) });
	Vertices.push_back(Vertex{ DirectX::XMFLOAT3(-1.f,-1.f, 1.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT4(DirectX::Colors::Green) });
	Vertices.push_back(Vertex{ DirectX::XMFLOAT3(-1.f, 1.f, 1.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT4(DirectX::Colors::Green) });
	Vertices.push_back(Vertex{ DirectX::XMFLOAT3( 1.f, 1.f, 1.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT4(DirectX::Colors::Green) });
	Vertices.push_back(Vertex{ DirectX::XMFLOAT3( 1.f,-1.f, 1.f),DirectX::XMFLOAT3(0.f,0.f,0.f),DirectX::XMFLOAT2(0.f,0.f),DirectX::XMFLOAT4(DirectX::Colors::Green) });


	ComPtr<ID3D12Resource> VertexBufferOnGPU{ nullptr };
	ComPtr<ID3D12Resource> VertexBufferUploader{ nullptr };

	VertexBufferOnGPU = CreateDefaultBuffer(m_d3dDevice.Get(), m_d3dCommandList.Get(), Vertices.data(), static_cast<UINT64>(Vertices.size() * sizeof(Vertex)), VertexBufferUploader);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};

	VertexBufferView.BufferLocation = VertexBufferOnGPU->GetGPUVirtualAddress();
	VertexBufferView.SizeInBytes = static_cast<UINT>(Vertices.size() * sizeof(Vertex));
	VertexBufferView.StrideInBytes = sizeof(Vertex);

	// BindVAO 와 같은 역할 
	m_d3dCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	m_d3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	std::vector<std::uint16_t> Indices{};
	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(2);

	Indices.push_back(0);
	Indices.push_back(2);
	Indices.push_back(3);

	Indices.push_back(4);
	Indices.push_back(6);
	Indices.push_back(5);

	Indices.push_back(4);
	Indices.push_back(7);
	Indices.push_back(6);

	Indices.push_back(4);
	Indices.push_back(5);
	Indices.push_back(1);

	Indices.push_back(4);
	Indices.push_back(1);
	Indices.push_back(0);

	Indices.push_back(3);
	Indices.push_back(2);
	Indices.push_back(6);

	Indices.push_back(3);
	Indices.push_back(6);
	Indices.push_back(7);

	Indices.push_back(1);
	Indices.push_back(5);
	Indices.push_back(6);

	Indices.push_back(1);
	Indices.push_back(6);
	Indices.push_back(2);

	Indices.push_back(4);
	Indices.push_back(0);
	Indices.push_back(3);

	Indices.push_back(4);
	Indices.push_back(3);
	Indices.push_back(7);


	ComPtr<ID3D12Resource> IndexBufferOnGPU{ nullptr };
	ComPtr<ID3D12Resource> IndexBufferUploader{ nullptr };

	IndexBufferOnGPU = CreateDefaultBuffer(m_d3dDevice.Get(), m_d3dCommandList.Get(), Indices.data(),static_cast<UINT64>(Indices.size() * sizeof(std::uint16_t)),IndexBufferUploader );

	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	IndexBufferView.BufferLocation = IndexBufferUploader->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
	IndexBufferView.SizeInBytes = static_cast<UINT>(Indices.size() * sizeof(std::uint16_t));

}

void Scene::CreatePipeLineStateObject(){
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipeLineStateObjectDesc{};
	::ZeroMemory(&PipeLineStateObjectDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	CD3DX12_RASTERIZER_DESC RasterizerDesc(D3D12_DEFAULT);
	CD3DX12_BLEND_DESC BlendDesc(D3D12_DEFAULT);
	CD3DX12_DEPTH_STENCIL_DESC DepthStencilDesc(D3D12_DEFAULT);

	PipeLineStateObjectDesc.InputLayout = { m_d3dInputLayout.data(), static_cast<UINT>(m_d3dInputLayout.size()) };
	PipeLineStateObjectDesc.pRootSignature = m_d3dRootSignature.Get();
	PipeLineStateObjectDesc.VS = ::GetShaderByteCode(m_d3dVertexShader);
	PipeLineStateObjectDesc.PS = ::GetShaderByteCode(m_d3dPixelShader);
	PipeLineStateObjectDesc.RasterizerState = RasterizerDesc;
	PipeLineStateObjectDesc.BlendState = BlendDesc;
	PipeLineStateObjectDesc.DepthStencilState = DepthStencilDesc;
	PipeLineStateObjectDesc.SampleMask = UINT_MAX;
	PipeLineStateObjectDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipeLineStateObjectDesc.NumRenderTargets = 1;
	PipeLineStateObjectDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipeLineStateObjectDesc.SampleDesc.Count = m_b4xMsaa ? 4 : 1;
	PipeLineStateObjectDesc.SampleDesc.Quality = m_b4xMsaa ? (m_n4xMsaaQuality - 1) : 0;
	PipeLineStateObjectDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ThrowIfFailed(m_d3dDevice->CreateGraphicsPipelineState(&PipeLineStateObjectDesc, IID_PPV_ARGS(&m_d3dPipeLineStateObject)));
}

ComPtr<ID3D12PipelineState> Scene::GetPipeLineStateObject() const
{
	return m_d3dPipeLineStateObject;
}

void Scene::Set4xMsaaState(bool b4xMsaa, UINT n4xMsaaQuality){
	m_b4xMsaa = b4xMsaa;
	m_n4xMsaaQuality = n4xMsaaQuality;
}

void Scene::Render(){
	ID3D12DescriptorHeap* DescriptorHeaps[] = { m_d3dShaderResourceDescriptorHeap.Get() };
	m_d3dCommandList->SetDescriptorHeaps(_countof(DescriptorHeaps), DescriptorHeaps);
	m_d3dCommandList->SetGraphicsRootSignature(m_d3dRootSignature.Get());
	

	m_d3dCommandList->SetGraphicsRootDescriptorTable(0, m_d3dShaderResourceDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	
}
