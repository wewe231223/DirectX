
struct cbMatrix
{
    float4x4 WorldViewProj;
};

ConstantBuffer<cbMatrix> ObjectConstant : register(b0);

struct VertexIN
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

struct VertexOUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

VertexOUT VertexShader(VertexIN Vin)
{
    VertexOUT Vout;
    
    
    Vout.Pos = mul(float4(Vin.Pos, 1.f), ObjectConstant.WorldViewProj);
    Vout.Color = Vin.Color;
    Vout.Normal = Vin.Normal;
    Vout.TexCoord = Vin.TexCoord;
    return Vout;
}

float4 PixelShader(VertexOUT Vout)
{
    return Vout.Color;
}