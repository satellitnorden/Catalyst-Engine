#if !defined(CATALYST_VECTOR_MATH)
#define CATALYST_VECTOR_MATH

/*
*   Performs a line/plane intersection. Returns if there was an intersection and will, in the event of an intersection distance, return the intersection point as well.
*/
bool LinePlaneIntersection(vec3 line_origin, vec3 line_direction, vec3 plane_position, vec3 plane_normal, out float intersection_distance)
{
    if (dot(line_direction, plane_normal) >= 0.0f)
    {
        return false;
    }

    else
    {
        intersection_distance = dot(plane_position - line_origin, plane_normal) / dot(line_direction, plane_normal);

        return true;
    }
}

/*
*   Performs a line/sphere intersection. Returns if there was an intersection and will, in the event of an intersection, return the intersection point as well.
*/
bool LineSphereIntersection(vec3 lineOrigin, vec3 lineDirection, vec3 spherePosition, float sphereRadius, out float T)
{
    vec3 L = lineOrigin - spherePosition;
    float B = 2.0f * dot(lineDirection, L);
    float C = dot(L, L) - sphereRadius * sphereRadius;

    float discriminant = B * B - 4.0f * C;

    if (discriminant <= 0.0f)
    {
        return false;
    }

    else
    {
        discriminant = sqrt(discriminant);
    }

    float T0 = -B + discriminant;
    float T1 = -B - discriminant;

    if (T0 <= 0.0f || T1 <= 0.0f)
    {
        return false;
    }

    T = T0 < T1 ? T0 * 0.5f : T1 * 0.5f;

    return true;
}

#endif