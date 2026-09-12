struct VS_OUTPUT
{
    float4 position_ : SV_POSITION;
    float4 color_    : TEXCOORD0;
};

float4 main(VS_OUTPUT input) : SV_Target
{
    return input.color_;
}
