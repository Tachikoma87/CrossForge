#version 330 core

in vec3 P_world;
flat in uint strand;
in vec3 T_world;
in vec3 color;
out vec4 fragColor;

struct DirLight
{
    vec3 direction;
    vec3 color;
};

struct PointLight
{
    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

#define PI 3.1415926535897932384626433832795

uniform int numLights;
uniform DirLight dirLights[2];
uniform PointLight pointLight;
uniform vec3 hairColor;
uniform vec3 viewPos;

uniform float specularExponent;

void main()
{
	V = normalize(viewPos - P_world);
	T = normalize(T_world);
	float sin_theta_o = dot(T, V);

	vec3 result = vec3(0.0);

	for (int l = 0; l < numLights; l++)
	{
		L = normalize(-dirLights[l].direction);
		vec3 lightColor = dirLights[l].color;

		float sin_theta_i = dot(T, L);

		// second and third vector of coordinate system
		L_p = normalize(L - (sin_theta_i * T));
		B = cross(L_p, T);

		vec3 diff = hairColor * intensity * dot(L, L_p);

		// kajiya lighting
		float spec = cos(acos(sin_theta_i) - (PI - acos(sin_theta_o)));
		if (spec < 0) {spec = 0;}
		spec = pow(spec, specularExponent);

		result = diff + (lightColor * spec);
	}
	fragColor.xyz /= numLights;
}


