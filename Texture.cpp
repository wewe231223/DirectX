#include "pch.h"
#include "Texture.h"

Texture::Texture(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12GraphicsCommandList> d3dCommandList, Path cPath) {
	std::tstring Ext = cPath.GetExtention();

#ifdef EXTENTION_NOT_INCLUDE_DOT
	if (Ext == _T("dds") or Ext == _T("DDS")) {
		ThrowIfFailed(DirectX::LoadFromDDSFile(cPath.C_Str(), DirectX::DDS_FLAGS_NONE, NULL, m_image));
	}
	else if (Ext == _T("tga") or Ext == _T("TGA")) {
		ThrowIfFailed(DirectX::LoadFromTGAFile(cPath.C_Str(), NULL, m_image));
	}
	else {
		ThrowIfFailed(DirectX::LoadFromWICFile(cPath.C_Str(), DirectX::WIC_FLAGS_NONE, NULL, m_image));
	}
#else 
	if (Ext == _T(".dds") or Ext == _T(".DDS")) {
		ThrowIfFailed(DirectX::LoadFromDDSFile(cPath.C_Str(), DirectX::DDS_FLAGS_NONE, NULL, m_image));
	}
	else if (Ext == _T(".tga") or Ext == _T(".TGA")) {
		ThrowIfFailed(DirectX::LoadFromTGAFile(cPath.C_Str(), NULL, m_image));
	}
	else {
		ThrowIfFailed(DirectX::LoadFromWICFile(cPath.C_Str(), DirectX::WIC_FLAGS_NONE, NULL, m_image));
	}
#endif // !EXTENTION_NOT_INCLUDE_DOT

	ThrowIfFailed(DirectX::CreateTexture(d3dDevice.Get(), m_image.GetMetadata(), m_d3dTextureResource.GetAddressOf()));
	std::vector<D3D12_SUBRESOURCE_DATA> SubResources{};
	ThrowIfFailed(DirectX::PrepareUpload(d3dDevice.Get(), m_image.GetImages(), m_image.GetImageCount(), m_image.GetMetadata(), SubResources));
	const UINT64 BufferSize = ::GetRequiredIntermediateSize(m_d3dTextureResource.Get(), 0, static_cast<UINT32>(SubResources.size()));


	m_d3dTextureResource = ::CreateDefaultBuffer(d3dDevice.Get(), d3dCommandList.Get(), SubResources, BufferSize, m_d3dTextureUploadBuffer);

};
