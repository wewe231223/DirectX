#pragma once
// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define FRAMEBUFFER_WIDTH 1920
#define FRAMEBUFFER_HEIGHT 1080
#define SWAPCHAINBUFFERCOUNT 2
#define DIRECTINPUT_VERSION 0x0800
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// C++ 런타임 헤더 파일입니다.
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>


// DirectX 런타임 헤더 파일입니다.
#include <wrl.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <dinput.h>


// DirectX 라이브러리 링크 
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")



using Microsoft::WRL::ComPtr;

struct _tagVertex {
	DirectX::XMFLOAT3 Position{};	// offset = 0 byte 
	DirectX::XMFLOAT3 Normal{};		// offset = 12 byte
	DirectX::XMFLOAT2 Tex{};		// offset = 24 byte 
	DirectX::XMFLOAT4 Color{};		// offset = 32 byte 
};									// total 48 byte 

using Vertex = _tagVertex;



#include "resource.h"
#include "Util.h"
#include "ConstantBuffer.h"



