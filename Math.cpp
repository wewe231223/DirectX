#include "pch.h"
#include "Math.h"

float Vector::Math::Radians(float vAngle){
    return vAngle * MathConstants::PI / 180.f;
}

DirectX::XMVECTOR Vector::Math::Radians(XMVECTOR vAngle){
    return XMVECTOR{ Radians(vAngle.m128_f32[0]), Radians(vAngle.m128_f32[1]), Radians(vAngle.m128_f32[2]), Radians(vAngle.m128_f32[3]) };
}
