#if !defined(CATALYST_GEOMETRY_MATH)
#define CATALYST_GEOMETRY_MATH

/////////////
// GENERAL //
/////////////

/*
*   Ray struct definition.
*/
struct Ray
{

    //The origin.
    vec3 _Origin;

    //The direction.
    vec3 _Direction;

};

/////////////
// SPHERES //
/////////////

/*
*   Sphere struct definition.
*/
struct Sphere
{

    //The position.
    vec3 _Position;

    //The radius.
    float _Radius;

};

//Ray/sphere intersection constants.
#define RAY_SPHERE_INTERSECTION_RESULT_NO_HIT (0)
#define RAY_SPHERE_INTERSECTION_RESULT_ONE_HIT (1)
#define RAY_SPHERE_INTERSECTION_RESULT_TWO_HITS (2)

/*
*   Ray/sphere intersection result struct definition.
*/
struct RaySphereIntersectionResult
{

    //The result.
    uint _Result;

    //The closest hit distance. Only set if _Result is either RAY_SPHERE_INTERSECTION_RESULT_ONE_HIT or RAY_SPHERE_INTERSECTION_RESULT_TWO_HITS.
    float _ClosestHitDistance;

    //The farthest hit distance. Only set if _Result is RAY_SPHERE_INTERSECTION_RESULT_TWO_HITS.
    float _FarthestHitDistance;

};

/*
*   Performs a ray/sphere intersection.
*/
RaySphereIntersectionResult RaySphereIntersection(Ray ray, Sphere sphere)
{
    RaySphereIntersectionResult result;

    float T0, T1;

    //Calculate TCA (distance from ray origin to sphere position).
    vec3 ray_origin_to_sphere_position = sphere._Position - ray._Origin; 
    float TCA = dot(ray_origin_to_sphere_position, ray._Direction); 

    //Early out.
    float d2 = dot(ray_origin_to_sphere_position, ray_origin_to_sphere_position) - TCA * TCA; 
    float radius_squared = sphere._Radius * sphere._Radius;

    if (d2 > radius_squared)
    {
        result._Result = RAY_SPHERE_INTERSECTION_RESULT_NO_HIT;

        return result; 
    }

    //Calculate THC (distance from closest hit distance to sphere position).
    float THC = sqrt(radius_squared - d2); 

    //Calculate the two hit distances.
    T0 = TCA - THC; 
    T1 = TCA + THC; 

    //Are both hit distances negative?
    if (T0 < 0.0f && T1 < 0.0f)
    {
        result._Result = RAY_SPHERE_INTERSECTION_RESULT_NO_HIT;

        return result; 
    }

    //Swap if necessary.
    if (T0 < 0.0f || (T0 > T1 && T1 > 0.0f))
    {
        float temporary = T0;
        T0 = T1;
        T1 = temporary;
    }
 
    //Is either hit distance negative?
    if (T0 < 0.0f)
    { 
        //If so, swap.
        T0 = T1;

        //Are both hit distances negative?
        if (T0 < 0.0f)
        {
            result._Result = RAY_SPHERE_INTERSECTION_RESULT_NO_HIT;

            return result; 
        }
    } 
 
    //Write the final result.
    result._Result = T1 < 0.0f ? RAY_SPHERE_INTERSECTION_RESULT_ONE_HIT : RAY_SPHERE_INTERSECTION_RESULT_TWO_HITS;
    result._ClosestHitDistance = T0;
    result._FarthestHitDistance = T1;

    return result;
}

#endif