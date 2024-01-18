#include "pch.h"
#include "Util.h"
#include <comdef.h>


WNDCLASSEXW GetDefaultWindowProperties(HINSTANCE hInstance){
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = nullptr;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECTX));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = nullptr;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return wcex;
}


ComPtr<ID3D12Resource> CreateDefaultBuffer(
    ID3D12Device* d3dDevice, 
    ID3D12GraphicsCommandList* d3dCommandList, 
    const void* Data, 
    UINT64 nByteSize, 
    ComPtr<ID3D12Resource>& d3dpUploadBuffer){
    

    ComPtr<ID3D12Resource> DefaultBuffer{ nullptr };

    CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(nByteSize);
    ThrowIfFailed(d3dDevice->CreateCommittedResource(
        &HeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &BufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(DefaultBuffer.GetAddressOf())
    ));
    

    HeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    ThrowIfFailed(d3dDevice->CreateCommittedResource(
        &HeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &BufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(d3dpUploadBuffer.GetAddressOf())   
    ));
    

    D3D12_SUBRESOURCE_DATA SubResourceData{};
    SubResourceData.pData = Data;
    SubResourceData.RowPitch = nByteSize;
    SubResourceData.SlicePitch = SubResourceData.RowPitch;

    CD3DX12_RESOURCE_BARRIER ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        DefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COPY_DEST
    );
    d3dCommandList->ResourceBarrier(1, &ResourceBarrier);
    UpdateSubresources<1>(d3dCommandList, DefaultBuffer.Get(), d3dpUploadBuffer.Get(), 0, 0, 1, &SubResourceData);
    ResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        DefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_GENERIC_READ
    );
    d3dCommandList->ResourceBarrier(1, &ResourceBarrier);

    return DefaultBuffer;
}
/// <summary>
/// Shader Compile 
/// </summary>
/// <param name="wcsFileName"> : File's Name</param>
/// <param name="d3dDefines"> : Ordinary takes 'nullptr'</param>
/// <param name="csEntryPoint"> : Shader's EntryPoint Function Name</param>
/// <param name="csTarget"> : Shader's Target( vs_5_0 ... )</param>
/// <returns></returns>
ComPtr<ID3DBlob> CompileShader(
    const std::wstring& wcsFileName, 
    const D3D_SHADER_MACRO* d3dDefines, 
    const std::string& csEntryPoint, 
    const std::string& csTarget){

    UINT CompileFlags{ 0 };
#if defined(DEBUG) || defined(_DEBUG)
    CompileFlags = D3DCOMPILE_DEBUG bitor D3DCOMPILE_SKIP_OPTIMIZATION;
#endif //!defined(DEBUG) | defined(_DEBUG)

    ComPtr<ID3DBlob> ByteCode{ nullptr };
    ComPtr<ID3DBlob> ErrorBlob{ nullptr };

    HRESULT hr = S_OK;

    hr = ::D3DCompileFromFile(wcsFileName.c_str(), d3dDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, csEntryPoint.c_str(), csTarget.c_str(), CompileFlags, 0, &ByteCode, &ErrorBlob);

    if (ErrorBlob != nullptr) ::OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());

    ThrowIfFailed(hr);

    return ByteCode;
}

D3D12_SHADER_BYTECODE GetShaderByteCode(ComPtr<ID3DBlob> d3dShaderBlob){
    return D3D12_SHADER_BYTECODE{ reinterpret_cast<BYTE*>(d3dShaderBlob->GetBufferPointer()),d3dShaderBlob->GetBufferSize() };
}

void BindVertexBuffer(ComPtr<ID3D12GraphicsCommandList> d3dCommandList,const D3D12_VERTEX_BUFFER_VIEW& d3dVertexBuffer,const D3D12_INDEX_BUFFER_VIEW& d3dIndexBuffer, D3D_PRIMITIVE_TOPOLOGY d3dTopology){
    d3dCommandList->IASetVertexBuffers(0, 1, &d3dVertexBuffer);
    d3dCommandList->IASetIndexBuffer(&d3dIndexBuffer);
    d3dCommandList->IASetPrimitiveTopology(d3dTopology);
}

Exeption::Exeption(HRESULT hErrorCode, const std::wstring& wcsFuctionName, const std::string& csFileName, const int nLineNumber):
    m_hErrorCode(hErrorCode), m_wsFunctionName(wcsFuctionName), m_sFileName(csFileName), m_nLineNumber(nLineNumber){
}

std::wstring Exeption::ToString() const{
    _com_error err(m_hErrorCode);
    std::wstring errmsg = err.ErrorMessage();
    WCHAR buffer[512];
    ::MultiByteToWideChar(CP_ACP, 0, m_sFileName.c_str(), -1, buffer, 512);
    std::wstring wsFileName{buffer};


    return m_wsFunctionName + L"\nFailed in : " + wsFileName + L"\nLine : " + std::to_wstring(m_nLineNumber) + L"\nError : " + errmsg;
}

namespace ApplicationFunctions {
    extern WindowInfo GetMainApplicationInfo();

    WindowInfo GetMainApplicationWindowInfo(){
        return GetMainApplicationInfo();
    }
}
