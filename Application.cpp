#include "pch.h"
#include "Application.h"
static Application* mApp = nullptr;

Application::Application(HINSTANCE hInstance,LPCWSTR wcpWindowName){
	m_hInstance = hInstance;
    WCHAR ClassName[100]{};
    ::LoadStringW(m_hInstance, IDC_DIRECTX, ClassName, 100);
    WNDCLASSEXW wcex = ::GetDefaultWindowProperties(m_hInstance);
    wcex.lpszClassName = ClassName;
    wcex.lpfnWndProc = Application::Procedure;
    ::RegisterClassExW(&wcex);
    m_hWnd = ::CreateWindowExW(0L, ClassName, wcpWindowName, WS_OVERLAPPEDWINDOW,
        100, 100, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 
        nullptr, nullptr, m_hInstance, nullptr);
    if (!m_hWnd) {
        exit(::GetLastError());
    }
    m_hHaccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDC_DIRECTX));
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);
}

Application::Application(HINSTANCE hInstance, LPCWSTR wcpWindowName, WNDCLASSEXW* pWindowProperties){
    m_hInstance = hInstance;
    WCHAR ClassName[100]{};
    ::LoadStringW(m_hInstance, IDC_DIRECTX, ClassName, 100);
    ::RegisterClassExW(pWindowProperties);
    m_hWnd = ::CreateWindowExW(0L, ClassName, wcpWindowName, WS_OVERLAPPEDWINDOW,
        100, 100, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT,
        nullptr, nullptr, m_hInstance, nullptr);
    if (!m_hWnd) {
        exit(::GetLastError());
    }
    m_hHaccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDC_DIRECTX));
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);
}

LRESULT Application::Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;



    return 0;
}

void Application::Loop(){
    while (true) {
        if (PeekMessage(&m_mMsg, NULL, 0, 0, PM_REMOVE)) {
            if (m_mMsg.message == WM_QUIT) break;
            if (!::TranslateAccelerator(m_mMsg.hwnd, m_hHaccelTable, &m_mMsg)) {
                ::TranslateMessage(&m_mMsg);
                ::DispatchMessage(&m_mMsg);
            }
            else {
                Update(0.f);
                Render();
            }
        }
    }
}

bool SetMainApplication(Application* app){
    if (mApp) return false;
    mApp = app;
    return true;
}
