#pragma once
class Util{

};

class Exeption {
public:
	Exeption() = default;
	Exeption(HRESULT hErrorCode, const std::tstring& wcsFuctionName, const std::string& csFileName, const int nLineNumber);

	std::tstring ToString() const;

	HRESULT m_hErrorCode{ S_OK };
	std::tstring m_wsFunctionName{};
	std::string m_sFileName{};
	int m_nLineNumber{};
};
#define ThrowIfFailed(hr){											\
	HRESULT hr_ = (hr);												\
	if(FAILED(hr_)){ throw Exeption(hr_,L#hr,__FILE__,__LINE__);}	\
																	\
}
#define EXTENTION_NOT_INCLUDE_DOT 
class Path {
public:
	Path(std::tstring tsPath);
private:
	std::tstring m_tsPath{};
	std::tstring m_tsExtention{};
public:
	operator std::tstring();
public:
	std::tstring GetExtention() const;
	const TCHAR* C_Str() const;
};



WNDCLASSEXW GetDefaultWindowProperties(HINSTANCE hInstance);
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }

inline UINT CalcConstantBufferByteSize(UINT Byte) {
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

ComPtr<ID3D12Resource> CreateDefaultBuffer(
	ID3D12Device* d3dDevice,
	ID3D12GraphicsCommandList* d3dCommandList,
	std::vector<D3D12_SUBRESOURCE_DATA>& Data,
	UINT64 nByteSize,
	ComPtr<ID3D12Resource>& d3dpUploadBuffer);

constexpr DirectX::XMFLOAT4X4 Identity = {
	1.f,0.f,0.f,0.f,
	0.f,1.f,0.f,0.f,
	0.f,0.f,1.f,0.f,
	0.f,0.f,0.f,1.f
};

ComPtr<ID3DBlob> CompileShader(const std::tstring& wcsFileName, const D3D_SHADER_MACRO* d3dDefines, const std::string& csEntryPoint, const std::string& csTarget);
D3D12_SHADER_BYTECODE GetShaderByteCode(ComPtr<ID3DBlob> d3dShaderBlob);

void BindVertexBuffer(ComPtr<ID3D12GraphicsCommandList> d3dCommandList,const D3D12_VERTEX_BUFFER_VIEW& d3dVertexBuffer,const D3D12_INDEX_BUFFER_VIEW& d3dIndexBuffer, D3D_PRIMITIVE_TOPOLOGY d3dTopology);

namespace ApplicationUtil {
	const WindowInfo* GetMainApplicationWindowInfo();
	void ClipMouse(const WindowInfo* cpWindowInfo);
	void FreeMouse();
	void ShowCursor(bool bState);
}

