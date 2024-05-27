
// -------------------------------------------------------------------
//      Globals
// -------------------------------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

// -------------------------------------------------------------------
//      DepthStencilState
// -------------------------------------------------------------------
DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;

    //others are redundant because
    // StenciAnable is FALSE
    // (for demo purposes only)

    //StencilReadMask = 0x0F;
    //StencilWriteMask = 0x0F;

    //FrontFaceStencilFunc = always;
    //BackFaceStencilFunc = always;

    //FrontFaceStencilDepthFail = keep;
    //BackFaceStencilDepthFail = keep;

    //FrontFaceStencilPass = keep;
    //BackFaceStencilPass = keep;

    //FrontFaceStencilFail = keep;
    //BackFaceStencilFail = keep;
};

// -------------------------------------------------------------------
//      RasterizerState
// -------------------------------------------------------------------
RasterizerState gRasterizerState
{
    CullMode = none;
    FrontCounterClockwise = false; // default;
};

// -------------------------------------------------------------------
//      BlendState
// -------------------------------------------------------------------
BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

// -------------------------------------------------------------------
//      SamplerState
// -------------------------------------------------------------------
SamplerState gSamPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap; // or Mirro, Clamp, Border
    AddressV = Wrap; // or Mirro, Clamp, Border
};

SamplerState gSamLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap; // or Mirro, Clamp, Border
    AddressV = Wrap; // or Mirro, Clamp, Border
};

SamplerState gSamAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 16;
    AddressU = Wrap; // or Mirro, Clamp, Border
    AddressV = Wrap; // or Mirro, Clamp, Border
};

// -------------------------------------------------------------------
//      Vertex Shader
// -------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.UV = input.UV;
    return output;
}

// -------------------------------------------------------------------
//      Pixel Shader(s)
// -------------------------------------------------------------------
float3 PS_POINT(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(gSamPoint, input.UV).rgb;
}

float3 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(gSamLinear, input.UV).rgb;
}

float3 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
    return gDiffuseMap.Sample(gSamAnisotropic, input.UV).rgb;
}


// -------------------------------------------------------------------
//      Technique(s)
// -------------------------------------------------------------------
technique11 DefaultTechnique
{
    pass POINT_FILTER
    {
        //SetRasterizerState(gRasterizerState);
        //SetDepthStencilState(gDepthStencilState, 0);
        //SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_POINT()));
    }
    pass LINEAR_FILTER
    {
        //SetRasterizerState(gRasterizerState);
        //SetDepthStencilState(gDepthStencilState, 0);
        //SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
        SetRasterizerState(gRasterizerState);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_LINEAR()));
    }
    pass ANISOTROPIC_FILTER
    {
        //SetRasterizerState(gRasterizerState);
        //SetDepthStencilState(gDepthStencilState, 0);
        //SetBlendState(gBlendState, float4(0.f, 0.f, 0.f, 0.f), 0xFFFFFFFF);
        SetRasterizerState(gRasterizerState);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
    }
}