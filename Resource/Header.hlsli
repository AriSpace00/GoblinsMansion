Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txOpacity : register(t4);
Texture2D txMetalic : register(t5);
Texture2D txRoughness : register(t6);
Texture2D txShadow : register(t7);

TextureCube txEnvironment : register(t8);
TextureCube txIBL_Diffuse : register(t9);
TextureCube txIBL_Specular : register(t10);
Texture2D txIBL_SpecularBRDF_LUT : register(t11);
Texture2D txRenderMap : register(t12);

SamplerState samplerLinear : register(s0);
SamplerState samplerClamp : register(s1);

static const uint PointLightCount = 5;

cbuffer ProjectionBuffer : register(b0)
{
    matrix Projection;
    matrix ShadowProjection;
    float3 CameraPos;
    float pad;
}

cbuffer ViewBuffer : register(b1)
{
    matrix View;
    matrix ShadowView;
    
}
cbuffer WorldBuffer : register(b2)
{
    matrix World;
}

cbuffer LightBuffer : register(b3)
{
    float4 Direction;
    float3 Color;
    float DL_pad;
}

cbuffer PointLight : register(b4)
{
    float ConstantTerm;
    float LinearTerm;
    float QuadraticTerm;
    float Pad0;

    struct
    {
        float3 PointLightPos;
        float Radius;
        float3 PointLightColor;
        float Intensity;
    } pointLights[PointLightCount];
}

cbuffer BallBuffer : register(b5)
{
    float SphereMetalic;
    float SphereRoughness;
    float SphereAmbient;
    bool useIBL;
}

struct ENVIRONMENT_PS_INPUT
{
    float4 posProjection : SV_POSITION;
    float3 TexCoords : TEXCOORD;
};

struct STATIC_INPUT
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct Outline_PS_INPUT
{
    float4 pos : SV_Position;
};

struct PS_INPUT
{
    float4 PosProjection : SV_POSITION;
    float3 PosWorld : POSITION;
    float2 Texcoord : TEXCOORD0;
    float3 NorWorld : NORMAL;
    float3 TanWorld : TANGENT;
    float4 PosShadow : TEXCOORD1;
};