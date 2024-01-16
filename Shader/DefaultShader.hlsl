
struct cbMatrix
{
    float4x4 WorldViewProj;
};

struct world
{
    float4x4 World;
};
ConstantBuffer<cbMatrix> ObjectConstant : register(b0);
ConstantBuffer<world> WorldConstant : register(b1);

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

VertexOUT DefaultVertexShader(VertexIN Vin)
{
    VertexOUT Vout;
    
    float4 PosW = mul(float4(Vin.Pos, 1.f), WorldConstant.World);
    Vout.PosL = mul(PosW, ObjectConstant.WorldViewProj);
    
    Vout.Normal = Vin.Normal;
    Vout.TexCoord = Vin.TexCoord;
    Vout.Color = Vin.Color;
    
    return Vout;
}

float4 DefaultPixelShader(VertexOUT Vout)  : SV_Target
{
    return Vout.Color;
}