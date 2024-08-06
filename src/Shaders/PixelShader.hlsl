struct VS_OUTPUT {
    float4 position : SV_POSITION; // Transformed position
    float4 color    : COLOR;       // Passed color
};

float4 main(VS_OUTPUT input) : SV_Target
{
    // Return the color as the output pixel color
    return input.color;
}