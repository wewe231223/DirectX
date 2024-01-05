
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
    float4 PosL : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
};

VertexOUT VS(VertexIN Vin)
{
    VertexOUT Vout;
    
    
    Vout.PosL = mul(float4(Vin.Pos, 1.0f), ObjectConstant.WorldViewProj);
    Vout.Normal = Vin.Normal;
    Vout.TexCoord = Vin.TexCoord;
    Vout.Color = Vin.Color;
    
    return Vout;
}

float4 PS(VertexOUT Vout)  : SV_Target
{
    return Vout.Color;
}