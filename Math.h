#pragma once

/// <summary>
/// Vector 와 관련된 모든 함수 정의, 외부에서 사용할 함수는 public 으로 선언할 것 
/// </summary>
namespace Vector {
	using namespace DirectX;
	class Math {
	public:
		static float Radians(float vAngle);
		static XMVECTOR Radians(XMVECTOR vAngle);
	};

}

/// <summary>
/// Matrix 와 관련된 모든 함수 정의, 외부에서 사용할 함수는 public 으로 선언할 것 
/// </summary>
namespace Matrix{
	class Math {
	public:

	};

}

namespace MathConstants {
	constexpr float PI = 3.141592f;
}