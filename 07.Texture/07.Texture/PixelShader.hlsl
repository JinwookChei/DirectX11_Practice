Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    
    float4 lightColor;
    float4 ambientColor;
    float3 spotPosition;
    float spotRange;
    float3 spotDirection;
    float spotAngle;
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // g_Sampler의 정보를 이용하여 Texture와 input.uv의 좌표 
    float4 texColor = g_Texture.Sample(g_Sampler, input.uv);
    
    float3 N = normalize(input.normal);
    float3 L = normalize(spotPosition - input.worldPos);
    float3 S = normalize(-spotDirection);
    
    float spotCos = dot(L, S);
    float spotEffect = smoothstep(spotAngle, spotAngle + 0.05, spotCos);
    
    float dist = length(spotPosition - input.worldPos);
    float att = saturate(1.0f - dist / spotRange);
    
    float diffuse = saturate(dot(N, L)) * spotEffect * att;
    float diffuseColor = diffuse * lightColor;
    
    // **Change**
    //float4 finalColor = input.color * (diffuseColor + ambientColor);
    //float4 finalColor = texColor * (diffuseColor + ambientColor)
    float4 finalColor = texColor;
    return finalColor;
}


//Debug Normal
//float4 main(PS_INPUT input) : SV_TARGET
//{
//    return float4(input.normal, 1.0f);
//}