// DirectX.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "Application.h"


std::unique_ptr<DirectXApplication> DefaultApp{nullptr};

#if defined(DEBUG) | defined(_DEBUG) 
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif // !defined(DEBUG) | defined(_DEBUG) 

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow){
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //!defined(DEBUG) | defined(_DEBUG)
    LPCWSTR WindowName = L"Application";
    try {
        DefaultApp = std::make_unique<DirectXApplication>(hInstance,WindowName);
        ApplicationUtil::SetMainApplication(DefaultApp.get());
        DefaultApp->Initialize();
        DefaultApp->Loop();
    }
    catch (Exeption& e) {
        ::MessageBox(0, e.ToString().c_str(), 0, 0);
    }    
    return 0;
}



