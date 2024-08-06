struct VS_INPUT {
    float3 position : POSITION; // Vertex position
};

// Output structure
struct VS_OUTPUT {
    float4 position : SV_POSITION; // Transformed position
    float4 color    : COLOR;       // Passed color
};

// Shader main function
VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    // Convert 3D position to homogeneous clip space (4D)
    output.position = float4(input.position, 1.0f);

    // Pass through the color
    output.color = output.position;

    return output;
}