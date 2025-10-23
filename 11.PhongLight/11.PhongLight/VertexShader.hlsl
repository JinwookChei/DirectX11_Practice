cbuffer CBTransform : register(b0)
{
    matrix cb_World;
    matrix cb_View;
    matrix cb_Projection;
    matrix cb_WorldInvTranspose;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 tangent : TANGENT;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD0;
    float2 uv : TEXCOORD1;
    float3x3 TBN : TEXCOORD2;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    float4 worldPosition = mul(float4(input.position, 1.0f), cb_World);     // ���� ������ ( ������ �������� �󸶸�ŭ �������ֳ� )
    float4 viewPosition = mul(worldPosition, cb_View);                      // �� ������ ( ī�޶� �������� �� ������ ) ( ī�޶� �����̶�? ī�޶��� �������� 0, 0, 0 ���� ���� ) ( ī�޶� �����̶�? ī�޶� �������� �����. )
    output.pos = mul(viewPosition, cb_Projection);
    output.color = input.color;
    
    float3 N = normalize(mul(input.normal, (float3x3) cb_World));
    float3 T = normalize(mul(input.tangent.xyz, (float3x3) cb_World));
    float3 B = normalize(cross(N, T) * input.tangent.w);
    
    output.normal = mul(float4(input.normal, 1.0f), cb_WorldInvTranspose);
    output.worldPos = worldPosition.xyz;
    output.uv = input.uv;
    output.TBN = float3x3(T, B, N);
    
    return output;
}
