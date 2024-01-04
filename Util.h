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