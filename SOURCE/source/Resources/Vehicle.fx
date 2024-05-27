
// -------------------------------------------------------------------
//      Globals
// -------------------------------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;
float3 gCameraPos : CAMERA;
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
static const float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);
static const float3 gAmbientColor = float3(0.03f, 0.03f, 0.03f);

static const float gDIV_PI = 0.3183098862f;
static const float gLightIntensity = 7.f;
static const float gShininess = 25.f;
static const float gFLT_EPSILON = 1.192092896e-07F;


// -------------------------------------------------------------------
//      Input/Output Structs
// -------------------------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
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
//      Shader Functions
// -------------------------------------------------------------------
float ObservedArea(float3 normal, float3 lightDirection)
{
    return float(saturate(dot(normal.xyz, lightDirection)));
}

float3 Lambert(float lightIntensity, float3 diffusedSample)
{
    return float3(diffusedSample * lightIntensity * gDIV_PI);
}

float3 Phong(float3 specularColor, float glossiness, float3 lightDirection, float3 vieuwDirction, float3 normal)
{
    const float cosA = float(dot(reflect(lightDirection, normal.xyz), vieuwDirction));
    if(cosA < gFLT_EPSILON)
        return float3(0.f, 0.f, 0.f);
    return float3((specularColor * pow(cosA, glossiness)));
}

// -------------------------------------------------------------------
//      Vertex Shader
// -------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.WorldPosition = mul(float4(input.Position, 1.f), gWorldMatrix);
    output.UV = input.UV;
    output.Normal = mul(float4(input.Normal, 1.f), gWorldMatrix);
    output.Tangent = mul(float4(input.Tangent, 1.f), gWorldMatrix);
    return output;
}

// -------------------------------------------------------------------
//      Pixel Shader(s)
// -------------------------------------------------------------------
float3 PS_POINT(VS_OUTPUT input) : SV_TARGET
{   
    // Normal
    const float3 biNormal = cross(input.Normal, input.Tangent);
    const float3x3 tangentSpaceAxis = float3x3(input.Tangent, biNormal, input.Normal);
    float3 normalMapSample = gNormalMap.Sample(gSamPoint, input.UV);
    normalMapSample *= 2.f;
    normalMapSample *= -1.f;
    const float3 normal = mul(normalize(normalMapSample), tangentSpaceAxis);
    
    // OA
    const float observedArea = ObservedArea(normal, gLightDirection);
    
    // Lambert
    const float3 diffusedMapSample = gDiffuseMap.Sample(gSamPoint, input.UV).rgb;
    const float3 lambert = Lambert(gLightIntensity, diffusedMapSample);
    
    // Viewdirection
    const float3 viewDirection = normalize(input.WorldPosition.xyz - gCameraPos);
    
    // Phong
    const float3 specularMapSample = gSpecularMap.Sample(gSamPoint, input.UV).rgb;
    const float glossinessMapSample = gGlossinessMap.Sample(gSamPoint, input.UV).r;
    const float3 specular = Phong(specularMapSample, glossinessMapSample, gLightDirection, viewDirection, normal).rgb;
    
    return float3(gAmbientColor + ((lambert + specular) * observedArea));
}

float3 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
    // Normal
    const float3 biNormal = cross(input.Normal, input.Tangent);
    const float3x3 tangentSpaceAxis = float3x3(input.Tangent, biNormal, input.Normal);
    float3 normalMapSample = gNormalMap.Sample(gSamLinear, input.UV);
    normalMapSample *= 2.f;
    normalMapSample *= -1.f;
    const float3 normal = mul(normalize(normalMapSample), tangentSpaceAxis);
    
    // OA
    const float observedArea = ObservedArea(normal, gLightDirection);
    
    // Lambert
    const float3 diffusedMapSample = gDiffuseMap.Sample(gSamLinear, input.UV).rgb;
    const float3 lambert = Lambert(gLightIntensity, diffusedMapSample);
    
    // Viewdirection
    const float3 viewDirection = normalize(input.WorldPosition.xyz - gCameraPos);
    
    // Phong
    const float3 specularMapSample = gSpecularMap.Sample(gSamLinear, input.UV).rgb;
    const float glossinessMapSample = gGlossinessMap.Sample(gSamLinear, input.UV).r;
    const float3 specular = Phong(specularMapSample, glossinessMapSample, gLightDirection, viewDirection, normal).rgb;
    
    return float3(gAmbientColor + ((lambert + specular) * observedArea));
}

float3 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
    // Normal
    const float3 biNormal = cross(input.Normal, input.Tangent);
    const float3x3 tangentSpaceAxis = float3x3(input.Tangent, biNormal, input.Normal);
    float3 normalMapSample = gNormalMap.Sample(gSamAnisotropic, input.UV);
    normalMapSample *= 2.f;
    normalMapSample *= -1.f;
    const float3 normal = mul(normalize(normalMapSample), tangentSpaceAxis);
    
    // OA
    const float observedArea = ObservedArea(normal, gLightDirection);
    
    // Lambert
    const float3 diffusedMapSample = gDiffuseMap.Sample(gSamAnisotropic, input.UV).rgb;
    const float3 lambert = Lambert(gLightIntensity, diffusedMapSample);
    
    // Viewdirection
    const float3 viewDirection = normalize(input.WorldPosition.xyz - gCameraPos);
    
    // Phong
    const float3 specularMapSample = gSpecularMap.Sample(gSamAnisotropic, input.UV).rgb;
    const float glossinessMapSample = gGlossinessMap.Sample(gSamAnisotropic, input.UV).r;
    const float3 specular = Phong(specularMapSample, glossinessMapSample, gLightDirection, viewDirection, normal).rgb;
    
    return float3(gAmbientColor + ((lambert + specular) * observedArea));
}

// -------------------------------------------------------------------
//      Technique(s)
// -------------------------------------------------------------------
technique11 DefaultTechnique
{
    pass POINT_FILTER
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_POINT() ) );
    }
    pass LINEAR_FILTER
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_LINEAR()));
    }
    pass ANISOTROPIC_FILTER
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
    }
}