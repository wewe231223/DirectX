#pragma once

struct ObjectConstants {
	DirectX::XMFLOAT4X4 WorldViewProjection{ Identity };
};
class Mesh;
class Camera;

class Scene{
public:
	Scene();
	virtual ~Scene();

public:
	void Initialize(ComPtr<ID3D12Device> d3dDevice,ComPtr<ID3D12GraphicsCommandList> d3dCommandList);
private:
	ComPtr<ID3D12Device> m_d3dDevice{ nullptr };
	ComPtr<ID3D12GraphicsCommandList> m_d3dCommandList{ nullptr };
	ComPtr<ID3D12RootSignature> m_d3dRootSignature{ nullptr };
	ComPtr<ID3D12DescriptorHeap> m_d3dShaderResourceDescriptorHeap{ nullptr };

	ComPtr<ID3DBlob> m_d3dVertexShader{ nullptr };
	ComPtr<ID3DBlob> m_d3dPixelShader{ nullptr };
	ComPtr<ID3D12PipelineState> m_d3dPipeLineStateObject{ nullptr };

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_d3dInputLayout{};
	
	ComPtr<ID3D12Resource> m_d3dConstantBuffer{ nullptr };

	bool m_b4xMsaa{ false };
	UINT m_n4xMsaaQuality{ 0 };

	std::unique_ptr<Camera> m_camera{ nullptr };
	std::unique_ptr<Mesh> mesh{ nullptr };


	D3D12_VERTEX_BUFFER_VIEW TestVertexBuffer{}; // VertexBufferView 
	D3D12_INDEX_BUFFER_VIEW TestIndexBuffer{}; // VertexBufferView 
	ObjectConstants* TestConstant{}; // ConstantBuffer Pointer 

	const ApplicationUtil::WindowInfo* m_windowInfo{ nullptr };

private:
	virtual void CreateShaderResourceDescriptorHeaps();
	virtual void CreateConstantBuffers();
	virtual void CreateRootSignature();
	virtual void CreateShaders();
	virtual void CreateInputLayout();
	virtual void CreateGeometry();
	virtual void CreatePipeLineStateObject();

public:
	ComPtr<ID3D12PipelineState> GetPipeLineStateObject() const ;
	void Set4xMsaaState(bool b4xMsaa, UINT n4xMsaaQuality);
	virtual void Render();
	virtual void Update(float fDeltaTime);
};	

