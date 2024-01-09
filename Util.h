#pragma once
class Util{

};

class Exeption {
public:
	Exeption() = default;
	Exeption(HRESULT hErrorCode, const std::wstring& wcsFuctionName, const std::string& csFileName, const int nLineNumber);

	std::wstring ToString() const;

	HRESULT m_hErrorCode{ S_OK };
	std::wstring m_wsFunctionName{};
	std::string m_sFileName{};
	int m_nLineNumber{};
};
#define ThrowIfFailed(hr){											\
	HRESULT hr_ = (hr);												\
	if(FAILED(hr_)){ throw Exeption(hr_,L#hr,__FILE__,__LINE__);}	\
																	\
}

WNDCLASSEXW GetDefaultWindowProperties(HINSTANCE hInstance);
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }

constexpr UINT CalcConstantBufferByteSize(UINT Byte) {
	/*
	* 들어온 크기를 0xff(256 바이트)의 배수로 만들어준다
	* Byte = 300
	* (300 + 255) = 555
	* 0x022b & ~0x00ff
	* 0x022b & 0xff00
	* 0x0200 = 512
	*/
	return (Byte + 0xff) & ~0xff;
}

/// <summary>
/// 정보( 정점 등 ) 을 실제로 담고 있는 기본 버퍼를 생성한다 
/// </summary>
/// <param name="d3dDevice"> : 프로그램의 Device</param>
/// <param name="d3dCommandList"> : 프로그램의 CommandList</param>
/// <param name="Data"> : 정점 정보 포인터</param>
/// <param name="nByteSize"> : 정점 정보의 크기</param>
/// <param name="d3dpUploadBuffer"> : 업로드 버퍼( 정점 버퍼의 경우 GPU에 업로드하는 업로드 버퍼, 실제 정보를 담는 기본 버퍼 두 가지가 있어야 한다 ) </param>
/// <returns></returns>
ComPtr<ID3D12Resource> CreateDefaultBuffer(
	ID3D12Device* d3dDevice, 
	ID3D12GraphicsCommandList* d3dCommandList,
	const void* Data, 
	UINT64 nByteSize, 
	ComPtr<ID3D12Resource>& d3dpUploadBuffer);

constexpr DirectX::XMFLOAT4X4 Identity = {
	1.f,0.f,0.f,0.f,
	0.f,1.f,0.f,0.f,
	0.f,0.f,1.f,0.f,
	0.f,0.f,0.f,1.f
};

ComPtr<ID3DBlob> CompileShader(const std::wstring& wcsFileName, const D3D_SHADER_MACRO* d3dDefines, const std::string& csEntryPoint, const std::string& csTarget);
D3D12_SHADER_BYTECODE GetShaderByteCode(ComPtr<ID3DBlob> d3dShaderBlob);

void BindVertexBuffer(ComPtr<ID3D12GraphicsCommandList> d3dCommandList,const D3D12_VERTEX_BUFFER_VIEW& d3dVertexBuffer,const D3D12_INDEX_BUFFER_VIEW& d3dIndexBuffer, D3D_PRIMITIVE_TOPOLOGY d3dTopology);
