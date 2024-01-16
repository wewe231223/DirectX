#pragma once
template<typename T>
class ConstantBuffer{
public:
	ConstantBuffer();
	/// <summary>
	/// Root Descriptor( Inline Root Descriptor ) = 별도의 CBV 없이 이 자체가 하나의 CBV를 담당한다
	/// </summary>
	/// <param name="d3dDevice"> : App's Device </param>
	ConstantBuffer(ComPtr<ID3D12Device> d3dDevice); 
	/// <summary>
	/// Descriptor Table = 별도의 Table 을 통해 여러개의 CBV SRV UAV를 합쳐서 저장한다.
	/// </summary>
	/// <param name="d3dDevice"> : App's Device</param>
	/// <param name="nDeacriptorIndex"> : Descriptor 의 index( 몇 번째 Descriptor 인지 )</param>
	ConstantBuffer(ComPtr<ID3D12Device> d3dDevice,ComPtr<ID3D12DescriptorHeap> d3dResourceDescriptorHeap, UINT nDescriptorIncreaseMent,UINT nDeacriptorIndex);
	~ConstantBuffer();
private:
	ComPtr<ID3D12Resource> m_d3dConstantBuffer{ nullptr };
	T* m_constantInterface{ nullptr };
	size_t m_nBufferSize{ 0 };
	bool m_bMapped{ false };
public:
	void CopyData(const T& Data);
	D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAddress() const;
	void Release();
};

template<typename T>
ConstantBuffer<T>::ConstantBuffer(){
}

template<typename T>
ConstantBuffer<T>::ConstantBuffer(ComPtr<ID3D12Device> d3dDevice){
	UINT ElementByteSize = CalcConstantBufferByteSize(sizeof(T));

	CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ConstantResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(ElementByteSize);
	m_nBufferSize = sizeof(T);

	ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&HeapProperties, 
		D3D12_HEAP_FLAG_NONE, 
		&ConstantResourceDesc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		nullptr, 
		IID_PPV_ARGS(m_d3dConstantBuffer.GetAddressOf())));
}

template<typename T>
ConstantBuffer<T>::ConstantBuffer(ComPtr<ID3D12Device> d3dDevice,ComPtr<ID3D12DescriptorHeap> d3dResourceDescriptorHeap,UINT nDescriptorIncreaseMent,UINT nDeacriptorIndex){
	UINT ElementByteSize = CalcConstantBufferByteSize(sizeof(T));

	CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC ConstantResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(ElementByteSize);
	m_nBufferSize = sizeof(T);

	ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ConstantResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_d3dConstantBuffer.GetAddressOf())));

	D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferAddress = m_d3dConstantBuffer->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc{};
	ConstantBufferViewDesc.BufferLocation = ConstantBufferAddress;
	ConstantBufferViewDesc.SizeInBytes = ElementByteSize;

	auto Handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(d3dResourceDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	Handle.Offset(static_cast<INT>(nDeacriptorIndex), nDescriptorIncreaseMent);
	d3dDevice->CreateConstantBufferView(&ConstantBufferViewDesc, Handle);

}



template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer(){
	if (m_bMapped) m_d3dConstantBuffer->Unmap(0, nullptr);
}

template<typename T>
inline void ConstantBuffer<T>::CopyData(const T& Data) {
	ThrowIfFailed(m_d3dConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_constantInterface)));
	m_bMapped = true;
	::memcpy(m_constantInterface, &Data, m_nBufferSize);
	m_d3dConstantBuffer->Unmap(0, nullptr);
	m_bMapped = false;
}

template<typename T>
inline D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer<T>::GetVirtualAddress() const {
	return m_d3dConstantBuffer->GetGPUVirtualAddress();
}

template<typename T>
inline void ConstantBuffer<T>::Release(){
	if (m_bMapped) m_d3dConstantBuffer->Unmap(0, nullptr);
}
