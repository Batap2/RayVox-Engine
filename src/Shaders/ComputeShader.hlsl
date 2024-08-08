RWTexture2D<float4> framebuffer : register(u0);

// cbuffer CameraBuffer : register(b0)
// {
//     struct
//     {
//         float4x4 viewproj;
//         float3 pos;
//         float padding;
//     }cameraBuffer;
// };

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

Ray GenerateRay(uint2 pixelCoord, uint2 screenSize, float4x4 cameraViewProjMatrix)
{
    float2 ndc = float2(
        (float)pixelCoord.x / screenSize.x * 2.0 - 1.0,
        (float)pixelCoord.y / screenSize.y * 2.0 - 1.0
    );

    float4 worldPos = mul(float4(ndc, 0.0, 1.0), cameraViewProjMatrix);

    Ray ray;
    ray.origin = cameraViewProjMatrix[3].xyz;
    ray.direction = normalize(worldPos.xyz - ray.origin);

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

    Ray ray;
    ray.origin = float3(float(dt_id.x)/400 - 1, float(dt_id.y)/400 - 1, -10);
    ray.direction = float3(0,0,1);

    Hit h = IntersectRaySphere(ray, sphere);

    if(h.hit){
        finalColor = float4(h.normal,1);
    }

    framebuffer[screen_coord.xy] = finalColor;
}