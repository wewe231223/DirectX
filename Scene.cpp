#include "pch.h"
#include "Scene.h"
#include "Mesh.h"
#include "Camera.h"


Scene::Scene(){
	
}

Scene::~Scene(){
}

void Scene::Initialize(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12GraphicsCommandList> d3dCommandList){
	m_d3dDevice = d3dDevice;
	m_d3dCommandList = d3dCommandList;

	CreateShaderResourceDescriptorHeaps();
	CreateConstantBuffers();
	CreateRootSignature();
	CreateShaders();
	CreateInputLayout();
	CreatePipeLineStateObject();
	CreateGeometry();


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
	//UINT ElementByteSize = CalcConstantBufferByteSize(sizeof(ObjectConstants));


	//CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	//CD3DX12_RESOURCE_DESC ConstantResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(ElementByteSize);
	//ThrowIfFailed(
	//	m_d3dDevice->CreateCommittedResource(
	//		&HeapProperties,
	//		D3D12_HEAP_FLAG_NONE,
	//		&ConstantResourceDesc,
	//		D3D12_RESOURCE_STATE_GENERIC_READ,
	//		nullptr,
	//		IID_PPV_ARGS(m_d3dConstantBuffer.GetAddressOf()))
	//);
	CBuffer = std::make_unique<ConstantBuffer::DescriptorTable<ObjectConstants>>(m_d3dDevice,m_d3dShaderResourceDescriptorHeap);


	//D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferAdress = m_d3dConstantBuffer->GetGPUVirtualAddress();

	//D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc{};
	//ConstantBufferViewDesc.BufferLocation = ConstantBufferAdress;
	//ConstantBufferViewDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(ObjectConstants));

	//m_d3dDevice->CreateConstantBufferView(&ConstantBufferViewDesc, m_d3dShaderResourceDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void Scene::CreateRootSignature(){
	CD3DX12_ROOT_PARAMETER RootParameter[2];

	CD3DX12_DESCRIPTOR_RANGE ConstantBufferDescriptorTable;
	ConstantBufferDescriptorTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,1,0);
	
	
	RootParameter[0].InitAsDescriptorTable(1,&ConstantBufferDescriptorTable); // Root Descriptor
	RootParameter[1].InitAsConstants(16, 1); // Root Constant 

	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(2, RootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> SerializedRootSignature{ nullptr };
	ComPtr<ID3DBlob> ErrorBlob{ nullptr };

	ThrowIfFailed(D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, SerializedRootSignature.GetAddressOf(), ErrorBlob.GetAddressOf()));
	ThrowIfFailed(m_d3dDevice->CreateRootSignature(0, SerializedRootSignature->GetBufferPointer(), SerializedRootSignature->GetBufferSize(), IID_PPV_ARGS(m_d3dRootSignature.GetAddressOf())));


}

void Scene::CreateShaders(){
	
	m_d3dVertexShader = ::CompileShader(L"Shader//DefaultShader.hlsl",nullptr, "DefaultVertexShader", "vs_5_1");
	m_d3dPixelShader = ::CompileShader(L"Shader//DefaultShader.hlsl", nullptr, "DefaultPixelShader", "ps_5_1");

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

	/*
	* 여러개의 메쉬를 사용할 때 Buffer을 여러개 생성하는 것이 아닌 
	* 하나의 버퍼에 이어서 저장하고 시작점을 제시하는 방식으로 저장한다.
	* 
	* 관리를 어떻게 할 것인가? 
	* 
	* ==>	Mesh 에서 전체 메쉬의 정점과 인덱스를 관리한다.
	*		Mesh 개개인의 정보는 정점 시작 위치, 인덱스 시작 위치, 인덱스 길이( 결국에 인덱스와 정점 시작위치만 알면 정점의 총 길이는 알 필요가 없다 ) 뿐이다.
			안에 static 들이 있기 때문에 unique pointer 는 적절하지 않아보임. 
			
			모든 메쉬를 생성하고, 모든 메쉬를 생성한 후, UploadVertexBuffer, UploadIndexBuffer 를 호출할것
			( 예외처리를 해두었기 때문에 중간에 호출해도 상관없지만 뭐가 되었던 두 함수의 마지막 호출 시점을 기준으로 버퍼가 업로드된다 ) 
				ㄴ> 저 함수를 호출한다고 실제로 GPU 메모리에 올라가는 것이 아님. 올릴 예정인거고 실제 그리기연산이 한번 호출되어야 비로소 Default Heap 으로 취급된다 
	*/
	
	mesh = std::make_unique<Mesh>("Box", Vertices, Indices);
	mesh->UploadVertexBuffer(m_d3dDevice, m_d3dCommandList);
	mesh->UploadIndexBuffer(m_d3dDevice, m_d3dCommandList);
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
	PipeLineStateObjectDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
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

	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * 3.141582f, 1920.f / 1080.f, 1.f, 1000.f);

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(10.f, 10.f, 10.f, 1.f);
	DirectX::XMVECTOR at = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);

	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(eye, at, up);

	DirectX::XMMATRIX VP = V * P;
	ID3D12DescriptorHeap* DescriptorHeaps[] = { m_d3dShaderResourceDescriptorHeap.Get() };
	m_d3dCommandList->SetGraphicsRootSignature(m_d3dRootSignature.Get());
	m_d3dCommandList->SetDescriptorHeaps(_countof(DescriptorHeaps), DescriptorHeaps);


	ObjectConstants cp{};
	
	DirectX::XMStoreFloat4x4(&cp.WorldViewProjection, DirectX::XMMatrixTranspose(VP));
	CBuffer->CopyData(cp);

	DirectX::XMFLOAT4X4 m{Identity};
	CBuffer->BindCommandList(m_d3dCommandList, m_d3dShaderResourceDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0);
	m_d3dCommandList->SetGraphicsRoot32BitConstants(1, 16, reinterpret_cast<void*>(&m), 0);
	
	::BindVertexBuffer(m_d3dCommandList, mesh->GetVertexView(), mesh->GetIndexView(), D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_d3dCommandList->SetGraphicsRootDescriptorTable(0, m_d3dShaderResourceDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	m_d3dCommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

	// XMMatrix 자료형과 XMFloat4x4의 행렬 방향은 반대로 되어있다. 따라서 해당 자료형을 사용할 때에는 행렬을 Transpose 해서 사용해야한다
	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose( DirectX::XMMatrixTranslationFromVector(DirectX::FXMVECTOR{ -100.f,-101.f,-100.f })));
	m_d3dCommandList->SetGraphicsRoot32BitConstants(1, 16, reinterpret_cast<void*>(&m), 0);
	m_d3dCommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

}
	