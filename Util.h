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
	if(FAILED(hr)){ throw Exeption(hr_,L#hr,__FILE__,__LINE__);}	\
																	\
}

WNDCLASSEXW GetDefaultWindowProperties(HINSTANCE hInstance);
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
