RWTexture2D<float4> framebuffer : register(u0);

cbuffer CameraBuffer : register(b0)
{
    struct CameraBuffer
    {
        float3 pos;
        float Znear;
        float3 forward;
        float Zfar;
        float3 right;
        float fov;
    }cameraBuffer;
};

struct Ray
{
    float3 origin;
    float3 direction;
};

struct Hit
{
    bool hit;
    float distance;
    float3 hitPoint;
    float3 normal;
};

struct Sphere
{
    float3 center;
    float radius;
};

Ray GenerateRay(float2 pixelPos, float2 screenSize, float3 cameraPos,
                float3 forward, float3 right, float fov)
{
    float aspectRatio = screenSize.x / screenSize.y;

    float2 ndc = (pixelPos / screenSize) * 2.0f - 1.0f;

    ndc.x *= aspectRatio;

    float tanFov = tan(radians(fov) * 0.5f);

    float3 up = cross(right, forward);

    // Camera space coordinates
    float3 cameraSpaceDir = normalize(ndc.x * right * tanFov + ndc.y * up * tanFov + forward);

    // Construct the ray
    Ray ray;
    ray.origin = cameraPos;
    ray.direction = cameraSpaceDir;

    return ray;
}

Hit IntersectRaySphere(Ray ray, Sphere sphere)
{
    Hit result;
    result.hit = false;

    float3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant > 0.0f)
    {
        float sqrtDiscriminant = sqrt(discriminant);
        float t1 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t2 = (-b + sqrtDiscriminant) / (2.0f * a);

        // Find the closest positive intersection
        if (t1 > 0.0f || t2 > 0.0f)
        {
            float t = min(t1, t2);
            if (t > 0.0f)
            {
                result.hit = true;
                result.distance = t;
                result.hitPoint = ray.origin + t * ray.direction;
                result.normal = normalize(result.hitPoint - sphere.center);
            }
        }
    }

    return result;
}

[numthreads(8, 8, 1)]
void main(uint3 dt_id : SV_DispatchThreadID, uint3 group_id : SV_GroupID)
{
    uint2 screen_coord = uint2(dt_id.x, dt_id.y);
    uint width, height;
    framebuffer.GetDimensions(width, height);

    float4 finalColor = float4(0,0,0,1);

    Sphere sphere;
    sphere.center = float3(0,0,0);
    sphere.radius = 1;

    Ray ray = GenerateRay(screen_coord, uint2(width, height), cameraBuffer.pos,
    cameraBuffer.forward, cameraBuffer.right, cameraBuffer.fov);

    Hit h = IntersectRaySphere(ray, sphere);

    if(h.hit){
        finalColor = float4(h.normal,1);
    }

    framebuffer[screen_coord.xy] = finalColor;
}