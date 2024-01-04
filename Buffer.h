#pragma once
template<typename T>
class Buffer{
public:
	Buffer(ID3D12Device* d3dDevice, UINT nElementCount, bool bIsConstantBuffer);
	Buffer(const Buffer& other) = delete;
	Buffer& operator=(const Buffer& other) = delete;
	~Buffer();
private:
	ComPtr<ID3D12Resource> m_d3dUploadBuffer{ nullptr };
	BYTE* m_btMappedData{ nullptr };

	UINT m_nElementSize{ 0 };
	bool m_bIsConstantBuffer{ false };
public:
	ID3D12Resource* GetResource() const;
	void CopyData(int nElementIndex, const T& Data);
};

template<typename T>
inline Buffer<T>::Buffer(ID3D12Device* d3dDevice, UINT nElementCount, bool bIsConstantBuffer) : m_bIsConstantBuffer(bIsConstantBuffer){
	if (m_bIsConstantBuffer) m_nElementSize = CalcConstantBufferByteSize(sizeof(T));
	else m_nElementSize = sizeof(T);

	CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_nElementSize * nElementCount);
	ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&BufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_d3dUploadBuffer.GetAddressOf())
	));

	ThrowIfFailed(m_d3dUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_btMappedData)));
}

template<typename T>
inline Buffer<T>::~Buffer(){
	if (m_d3dUploadBuffer != nullptr) {
		m_d3dUploadBuffer->Unmap(0, nullptr);
	}
	m_d3dUploadBuffer = nullptr;
}

template<typename T>
inline ID3D12Resource* Buffer<T>::GetResource() const{
	return m_d3dUploadBuffer.Get();
}

template<typename T>
inline void Buffer<T>::CopyData(int nElementIndex, const T& Data){
	std::memcpy(&m_btMappedData[nElementIndex * m_nElementSize], &data, sizeof(T));
}
