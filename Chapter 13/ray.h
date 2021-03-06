
#ifndef RAYH
#define RAYH
#include "vec3.h"

float drand48()
{
	return float(rand()) / RAND_MAX;
}

class ray
{
public:
	ray() {}
	ray(const vec3& a, const vec3& b):origin(a),direction(b) { }
	vec3 point_at_parameter(float t) const { return origin + t * direction; }
	vec3 origin;
	vec3 direction;
};

#endif


