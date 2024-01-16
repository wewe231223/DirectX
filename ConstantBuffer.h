#pragma once
namespace ConstantBuffer {
	template<typename T>
	class RootDescriptorBuffer {
	public:
		RootDescriptorBuffer();
		/// <summary>
		/// Root Descriptor( Inline Root Descriptor ) = 별도의 CBV 없이 이 자체가 하나의 CBV를 담당한다
		/// </summary>
		/// <param name="d3dDevice"> : App's Device </param>
		RootDescriptorBuffer(ComPtr<ID3D12Device> d3dDevice);
		~RootDescriptorBuffer();
	private:
		ComPtr<ID3D12Resource> m_d3dConstantBuffer{ nullptr };
		T* m_constantInterface{ nullptr };
		size_t m_nBufferSize{ 0 };
		bool m_bMapped{ false };
	public:
		void CopyData(const T& Data);
		void BindCommandList(ComPtr<ID3D12GraphicsCommandList> d3dCommandList,UINT nRootSignatureIndex);
		void Release();
	};

	template<typename T>
	inline RootDescriptorBuffer<T>::RootDescriptorBuffer() {
	}

	template<typename T>
	inline RootDescriptorBuffer<T>::RootDescriptorBuffer(ComPtr<ID3D12Device> d3dDevice) {
		UINT ElementByteSize = CalcConstantBufferByteSize(sizeof(T));
		m_nBufferSize = sizeof(T);

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
	inline RootDescriptorBuffer<T>::~RootDescriptorBuffer() {
		if (m_bMapped) m_d3dConstantBuffer->Unmap(0, nullptr);
	}

	template<typename T>
	inline void RootDescriptorBuffer<T>::CopyData(const T& Data) {
		ThrowIfFailed(m_d3dConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_constantInterface)));
		m_bMapped = true;
		::memcpy(m_constantInterface, &Data, m_nBufferSize);
		m_d3dConstantBuffer->Unmap(0, nullptr);
		m_bMapped = false;
	}

	template<typename T>
	inline void RootDescriptorBuffer<T>::BindCommandList(ComPtr<ID3D12GraphicsCommandList> d3dCommandList,UINT nRootSignatureIndex){
		d3dCommandList->SetGraphicsRootConstantBufferView(nRootSignatureIndex, m_d3dConstantBuffer->GetGPUVirtualAddress());
	}

	template<typename T>
	inline void RootDescriptorBuffer<T>::Release() {
		if (m_bMapped) m_d3dConstantBuffer->Unmap(0, nullptr);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class DescriptorTable {
	public:
		DescriptorTable();
		DescriptorTable(ComPtr<ID3D12Device> d3dDevice,ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap);
	private:
		ComPtr<ID3D12Resource> m_d3dConstantBuffer{ nullptr };
		T* m_constantInterface{ nullptr };
		size_t m_nBufferSize{ 0 };
		bool m_bMapped{ false };
	public:
		void CopyData(const T& Data);
		void BindCommandList(ComPtr<ID3D12GraphicsCommandList> d3dCommandList,D3D12_GPU_DESCRIPTOR_HANDLE d3dGPUDescriptorHandle ,UINT nRootSignatureIndex);
		void Release();
	};

	

	template<typename T>
	inline DescriptorTable<T>::DescriptorTable(){
	}

	template<typename T>
	inline DescriptorTable<T>::DescriptorTable(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap){
		UINT ElementByteSize = CalcConstantBufferByteSize(sizeof(T));
		m_nBufferSize = sizeof(T);

		CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC ConstantResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(ElementByteSize);

		ThrowIfFailed(d3dDevice->CreateCommittedResource(
			&HeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&ConstantResourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(m_d3dConstantBuffer.GetAddressOf())));

		// Descruiptor Table 에서는 매우 중요한 숫자이다
		// CBV SRV UAV 를 바인딩한 개수대로 늘려줘야 다음 Descriptor 생성 시에 제대로된 위치에 Descriptor Table 을 생성 가능하다 
		static UINT NumOfResources = 0;

		D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferVirtualAddress = m_d3dConstantBuffer->GetGPUVirtualAddress();
		D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc{};
		::ZeroMemory(&ConstantBufferViewDesc, sizeof(D3D12_CONSTANT_BUFFER_VIEW_DESC));
		ConstantBufferViewDesc.BufferLocation = ConstantBufferVirtualAddress;
		ConstantBufferViewDesc.SizeInBytes = ElementByteSize;

		CD3DX12_CPU_DESCRIPTOR_HANDLE CPUHandle{ d3dDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		CPUHandle.Offset(NumOfResources, d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		d3dDevice->CreateConstantBufferView(&ConstantBufferViewDesc, CPUHandle);

		// 일단 상수 버퍼만을 취급하고 있으므로 1을 더해줬다 ( CBV 하나 ) 
		// 만약 여기에 SRV 도 같이 묶는다면, 1이 아니라 2을 더해줘야 한다 ( CBV, SRV 두개 ) 
		NumOfResources += 1;
	}

	template<typename T>
	inline void DescriptorTable<T>::CopyData(const T& Data){
		ThrowIfFailed(m_d3dConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_constantInterface)));
		m_bMapped = true;
		::memcpy(m_constantInterface, &Data, m_nBufferSize);
		m_d3dConstantBuffer->Unmap(0, nullptr);
		m_bMapped = false;
	}

	template<typename T>
	inline void DescriptorTable<T>::BindCommandList(ComPtr<ID3D12GraphicsCommandList> d3dCommandList, D3D12_GPU_DESCRIPTOR_HANDLE d3dGPUDescriptorHandle, UINT nRootSignatureIndex){
		d3dCommandList->SetGraphicsRootDescriptorTable(nRootSignatureIndex, d3dGPUDescriptorHandle);
	}

	template<typename T>
	inline void DescriptorTable<T>::Release(){
		if (m_bMapped) m_d3dConstantBuffer->Unmap(0, nullptr);
	}

}