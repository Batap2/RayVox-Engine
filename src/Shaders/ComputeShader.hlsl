RWTexture2D<float4> framebuffer : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 dt_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID)
{
    uint2 screen_coord = uint2(dt_id.x, dt_id.y);
    uint width, height;
    framebuffer.GetDimensions(width, height);

    uint2 group_coord = uint2(group_id.xy);

    framebuffer[screen_coord.xy] = float4(float(group_coord.x),float(group_coord.y),0,1);
}