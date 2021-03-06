#ifndef MATERIALH
#define MATERIALH

#include "ray.h"
#include "hitable.h"

float drand48()
{
	return float(rand()) / RAND_MAX;
}

vec3 random_in_unit_sphere()
{
	vec3 p;
	do
	{
		p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1.0, 1.0, 1.0);
	} while (p.squared_length() >= 1.0);

	return p;
}


class material {
	public:
		virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;

};

class lambertian : public material 
{
public:
	lambertian(const vec3& a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target-rec.p);
		attenuation = albedo;
		return true;
	}

	vec3 albedo;
};

vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2 * dot(v, n)*n;
}

class metal : public material
{
public:
	metal(const vec3& a, float f) : albedo(a), fuzz(f){ if (fuzz > 1.0) fuzz = 1.0; }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return true;
	}
	
	float fuzz;
	vec3 albedo;
};

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
	vec3 uv(unit_vector(v));
	float dt = dot(uv, n); // -cosa
	float discrimiant = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt); // cosb*cosb
	if (discrimiant > 0)
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discrimiant);
		return true;
	}

	return false;
}


class dielectric : public material
{
public:
	dielectric(float ri): ref_idx(ri) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction, rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		if (dot(r_in.direction, rec.normal) > 0)
		{
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
		}
		else
		{
			outward_normal = rec.normal;
			ni_over_nt = 1 / ref_idx;
		}
		if (refract(r_in.direction, outward_normal, ni_over_nt, refracted))
		{
			scattered = ray(rec.p, refracted);
		}
		else
		{
			scattered = ray(rec.p, reflected);
			return false;
		}

		return true;
	}

	float ref_idx;
};
#endif