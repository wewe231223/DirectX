#pragma once
class Texture{
public:
	Texture() = default;
	Texture(ComPtr<ID3D12Device> d3dDevice,ComPtr<ID3D12GraphicsCommandList> d3dCommandList,Path cPath);
private:
	ComPtr<ID3D12Resource> m_d3dTextureResource{ nullptr };
	ComPtr<ID3D12Resource> m_d3dTextureUploadBuffer{ nullptr };
	
	DirectX::ScratchImage m_image{};


};

