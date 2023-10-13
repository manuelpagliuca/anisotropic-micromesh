#include "Sources/BoundingBox.h"

BoundingBox::BoundingBox(vec3 point)
{
    init(point);
}

vec3 BoundingBox::center() const
{
    return mix(minPoint, maxPoint, 0.5f);
}

float BoundingBox::diagonal() const
{
    return length(minPoint - maxPoint);
}

float BoundingBox::radius() const
{
    return diagonal() / 2.0f;
}

int BoundingBox::maxAxis() const
{
    vec3 ext = maxPoint - minPoint;

    if (ext.x > ext.y && ext.x > ext.z)
        return 0;
    if (ext.y > ext.z)
        return 1;
    return 2;
}

void BoundingBox::init(const vec3 point)
{
    minPoint = maxPoint = point;
}

void BoundingBox::includeAnotherPoint(const vec3 point)
{
    minPoint = min(minPoint, point);
    maxPoint = max(maxPoint, point);
}

mat4 BoundingBox::centering() const
{
    return scale(vec3(1.0f / radius())) * translate(-center());
}

void BoundingBox::printf() const
{
    std::cout << "min " << to_string(minPoint) << std::endl;
    std::cout << "max " << to_string(minPoint) << std::endl;
    std::cout << "center " << to_string(center()) << std::endl;
    std::cout << "radius " << radius() << std::endl;
}
