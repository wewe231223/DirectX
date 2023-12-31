// DirectX.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "pch.h"
#include "Application.h"


std::unique_ptr<DirectXApplication> DefaultApp{nullptr};



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow){
    LPCWSTR WindowName = L"Application";
    DefaultApp = std::make_unique<DirectXApplication>(hInstance,WindowName);
    ApplicationFunctions::SetMainApplication(DefaultApp.get());
    try {
        DefaultApp->Loop();
    }
    catch (Exeption& e) {
        ::MessageBox(0, e.ToString().c_str(), 0, 0);
    }
    return 0;
}



