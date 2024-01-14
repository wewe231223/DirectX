#pragma once
template<typename T>
class ConstantBuffer{
public:
	ConstantBuffer();
	ConstantBuffer(ComPtr<ID3D12Device> d3dDevice);
	~ConstantBuffer();
private:
	ComPtr<ID3D12Resource> m_d3dConstantBuffer{ nullptr };
	T* m_constantInterface{ nullptr };
	bool m_bMapped{ false };
public:
	void CopyData(const T& Data);
	D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress() const;
};

template<typename T>
ConstantBuffer<T>::ConstantBuffer(){
}

template<typename T>
ConstantBuffer<T>::ConstantBuffer(ComPtr<ID3D12Device> d3dDevice){
	UINT ElementByteSize = CalcConstantBufferByteSize(sizeof(T));

	CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ConstantResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(ElementByteSize);

	ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&HeapProperties, 
		D3D12_HEAP_FLAG_NONE, 
		&ConstantResourceDesc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, 
		IID_PPV_ARGS(m_d3dConstantBuffer.GetAddressOf())));
}

template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer(){
	if (m_bMapped) m_d3dConstantBuffer->Unmap(0, nullptr);
}

template<typename T>
inline void ConstantBuffer<T>::CopyData(const T& Data) {
	T* Temp{nullptr};
	ThrowIfFailed(m_d3dConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&Temp)));
	m_bMapped = true;
	::memcpy(Temp, &Data, sizeof(T));
	m_d3dConstantBuffer->Unmap(0, nullptr);
	m_bMapped = false;
}

template<typename T>
inline D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer<T>::GetVirtualAddress() const {
	return m_d3dConstantBuffer->GetGPUVirtualAddress();
}