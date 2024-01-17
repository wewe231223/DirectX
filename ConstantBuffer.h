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
		RootDescriptorBuffer(ComPtr<ID3D12Device> d3dDevice,UINT nRootDescriptorIndex);
		~RootDescriptorBuffer();
	private:
		ComPtr<ID3D12Resource> m_d3dConstantBuffer{ nullptr };
		T* m_constantInterface{ nullptr };
		size_t m_nBufferSize{ 0 };
		UINT m_nRootSignatureIndex{ 0 };
		bool m_bMapped{ false };
	public:
		CD3DX12_ROOT_PARAMETER GetRootParameter(UINT nShaderRegister);
		void CopyData(const T& Data);
		void BindCommandList(ComPtr<ID3D12GraphicsCommandList> d3dCommandList);
		void Release();
	};

	template<typename T>
	inline RootDescriptorBuffer<T>::RootDescriptorBuffer() {
	}

	template<typename T>
	inline RootDescriptorBuffer<T>::RootDescriptorBuffer(ComPtr<ID3D12Device> d3dDevice, UINT nRootDescriptorIndex) {
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
	inline CD3DX12_ROOT_PARAMETER RootDescriptorBuffer<T>::GetRootParameter(UINT nShaderRegister)
	{
		CD3DX12_ROOT_PARAMETER RootParameter{};
		::ZeroMemory(&RootParameter, sizeof(CD3DX12_ROOT_PARAMETER));

		RootParameter.InitAsConstantBufferView(nShaderRegister);

		return RootParameter;
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
	inline void RootDescriptorBuffer<T>::BindCommandList(ComPtr<ID3D12GraphicsCommandList> d3dCommandList){
		d3dCommandList->SetGraphicsRootConstantBufferView(m_nRootSignatureIndex, m_d3dConstantBuffer->GetGPUVirtualAddress());
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
		DescriptorTable(ComPtr<ID3D12Device> d3dDevice,ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap, UINT nRootSignatureIndex);
		~DescriptorTable();
	private:
		ComPtr<ID3D12Resource> m_d3dConstantBuffer{ nullptr };
		T* m_constantInterface{ nullptr };
		size_t m_nBufferSize{ 0 };
		UINT m_nRootSignatureIndex{ 0 };
		std::vector<CD3DX12_DESCRIPTOR_RANGE> m_descriptorRange{};
		bool m_bMapped{ false };
	public:
		void CopyData(const T& Data);
		void BindCommandList(ComPtr<ID3D12GraphicsCommandList> d3dCommandList,D3D12_GPU_DESCRIPTOR_HANDLE d3dGPUDescriptorHandle);
		void Release();
	};

	

	template<typename T>
	inline DescriptorTable<T>::DescriptorTable(){
	}

	template<typename T>
	inline DescriptorTable<T>::DescriptorTable(ComPtr<ID3D12Device> d3dDevice, ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap, UINT nRootSignatureIndex){
		UINT ElementByteSize = CalcConstantBufferByteSize(sizeof(T));
		m_nBufferSize = sizeof(T);
		m_nRootSignatureIndex = nRootSignatureIndex;

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
		// 보통 하나의 Descriptor Table 에 하나의 리소스를 바인딩하는 것이 보통이므로 ( 왜냐하면 두개이상 바인딩할 경우 둘중 하나라도 업데이트 되는경우 둘다 업데이트 해줘야 한다 ) 
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
	inline DescriptorTable<T>::~DescriptorTable(){
		if (m_bMapped) m_d3dConstantBuffer->Unmap(0, nullptr);
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
	inline void DescriptorTable<T>::BindCommandList(ComPtr<ID3D12GraphicsCommandList> d3dCommandList, D3D12_GPU_DESCRIPTOR_HANDLE d3dGPUDescriptorHandle){
		d3dCommandList->SetGraphicsRootDescriptorTable(m_nRootSignatureIndex, d3dGPUDescriptorHandle);
	}

	template<typename T>
	inline void DescriptorTable<T>::Release(){
		if (m_bMapped) m_d3dConstantBuffer->Unmap(0, nullptr);
	}


	/* Root Descriptor vs Descriptor Table 
	* 
	* 통상적으로 Descriptor Table 에는 하나의 CBV/SRV/URV 를 바인딩한다.
	* 
	* 그렇다면 하나의 View 를 바인딩하는 Root Descriptor 과 다를게 무엇인가? 
	* 
	* 첫번째로 Root Signature 의 크기 제한이다.
	* 
	* Root Descriptor 은 Root 에 하나의 View 에 대한 Descriptor 을 직접 바인딩하는 것으로, 2DWORD 를 차지한다.
	* 
	* 하지만 Descriptor Table 간접참조 ( 포인터 ) 를 통해 자료에 접근하는 방식이므로, 1 DWORD 를 차지한다.
	* 
	* 따라서 Root Signature 의 64DWORD 의 크기 제한을 맞추기 위하여 Descriptor Table 을 사용해야 한다 
	* 
	* 두 번째로 텍스쳐와 Texture2D 와 같은 리소스들은 Root Descriptor 를 통해 사용 할 수 없다.
	* 
	* 하지만 Root Descriptor 은 Descriptor Table 와 달리 참조 횟수가 한차례 적으므로, ( Root Descriptor = Descriptor -> 실제 데이터 / Descriptor Table = Table -> Descriptor -> 실제 데이터 ) 
	* 비교적 높은 업데이트 성능을 제공한다. 
	* 
	* 이와 같은 이유로 통상적인 셰이더 상수 ( Uniform 과 같이 행렬, 벡터 등등 ) 들은 자주 변경되야 하므로,Root Descriptor 을 사용하여 전달하고, 
	* 이외의 텍스쳐와 같은 리소스를 Descriptor Heap 을 사용하여 전달한다. 
	*/
}
