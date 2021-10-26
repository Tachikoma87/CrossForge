#version 330 core 

#define DIRECTIONAL_LIGHTS 
#define POINT_LIGHTS 
#define SPOT_LIGHTS

// just PI
const float PI = 3.14159265359;

// image post processing
const float Exposure = 1.0;
const float Gamma = 2.2;
const float Saturation = 1.2;
const float Brightness = 1.05;
const float Contrast = 1.05;

// light defines
const uint DirLightCount = 2U;
const uint PointLightCount = 2U;
const uint SpotLightCount = 1U;


// shadow defines
#define PCF_SHADOWS // enable percentage closer filtering (PCF)
const int PCFFilterSize = 1;
const float ShadowBias = 0.0005;
const uint ShadowMapCount = 2U;


layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

#ifdef DIRECTIONAL_LIGHTS
layout(std140) uniform DirectionalLightsData{
	vec4 Directions[DirLightCount];
	vec4 Colors[DirLightCount]; ///< a component is intensity
	mat4 LightSpaceMatrices[DirLightCount];
	ivec4 ShadowIDs[DirLightCount];
}DirLights;
#endif

#ifdef POINT_LIGHTS
layout(std140) uniform PointLightsData{
	vec4 Position[PointLightCount];	
	vec4 Color[PointLightCount];
	vec4 Attenuation[PointLightCount];
	vec4 Direction[PointLightCount];
	mat4 LightSpaceMatrices[PointLightCount];
	ivec4 ShadowIDs[PointLightCount];
}PointLights;
#endif

#ifdef SPOT_LIGHTS
layout(std140) uniform SpotLightsData{
	vec4 Position[SpotLightCount];
	vec4 Direction[SpotLightCount]; // Direction.w is outer cutoff
	vec4 Color[SpotLightCount];	// Color.w is intensity 
	vec4 Attenuation[SpotLightCount]; // Attenuation.w is inner cutoff
	mat4 LightSpaceMatrices[SpotLightCount];
	ivec4 ShadowIDs[SpotLightCount];
}SpotLights;
#endif

in vec2 UV; 

uniform sampler2D TexDepth; // gBuffer position data 
uniform sampler2D TexAlbedo; // gBuffer albedo/spec data
uniform sampler2D TexNormal; // gBuffer normal data 
uniform sampler2D TexShadow[ShadowMapCount];

out vec4 FragColor;

vec3 adjustColorAttributes(vec3 Color, float Saturation, float Brightness, float Contrast){
	// adjust brightness 
	vec3 Rval =  vec3(Brightness) * Color;
	// adjust Contrast
	Rval = 0.5 + Contrast * (Rval - vec3(0.5));
	// adjust Saturation 
	const vec3 LuminosityFactor = vec3(0.2126, 0.7152, 0.0722);
	vec3 Grayscale = vec3(dot(Rval, LuminosityFactor));
	Rval = mix(Grayscale, Rval, Saturation);
	return Rval;
}//ajdustColorAttributes


void main(){
	vec3 WorldPos = texture(TexDepth, UV).rgb;
	vec3 N = texture(TexNormal, UV).rgb;
	vec3 Albedo = texture(TexAlbedo, UV).rgb;

	vec3 Lighting = Albedo * 0.1; // ambient 

	vec3 V = normalize(Camera.Position.xyz - WorldPos);


	for(uint i=0U; i < DirLightCount; ++i){
		vec3 L = -normalize(DirLights.Directions[i]).xyz;
		float DiffStrength = max(dot(N, L), 0.0);
		if(DiffStrength > 0.0){
			vec3 Diffuse =  0.6 * DiffStrength * Albedo * DirLights.Colors[i].xyz;

			vec3 H = normalize(V + L);
			vec3 Specular = DirLights.Colors[i].xyz * pow(max( dot(N,H), 0.0), 2.0);

			Lighting += Diffuse;
		}
		
	}//for[all lights]

	Lighting = adjustColorAttributes(Lighting, Saturation, Brightness, Contrast);

	FragColor = vec4(Lighting, 1.0);
}//main 