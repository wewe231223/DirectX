#pragma once
class Scene{
public:
	Scene();
	~Scene();

public:
	void Initialize(ComPtr<ID3D12Device> d3dDevice,ComPtr<ID3D12CommandList> d3dCommandList);
private:
	ComPtr<ID3D12RootSignature> m_d3dRootSignature{ nullptr };
	ComPtr<ID3D12DescriptorHeap> m_d3dConstantBufferViewHeap{ nullptr };

	ComPtr<ID3DBlob> m_d3dVertexShader{ nullptr };
	ComPtr<ID3DBlob> m_d3dFragmentShader{ nullptr };
	ComPtr<ID3D12PipelineState> m_d3dPipeLineStateObject{ nullptr };

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_d3dInputLayout{};

private:
	void CreateDescriptorHeaps();
	void CreateShaders();
	void CreateInputLayout();

public:

	ComPtr<ID3D12PipelineState> GetPipeLineStateObject() const ;

};	

