#version 330 core 

#define MULTIPLE_SHADOWS
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
const float ShadowBias = 0.0002;
const uint ShadowMapCount = 1U;

layout(std140) uniform CameraData{
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

in vec3 Pos;
in vec3 N;
in vec2 UV;

uniform sampler2D TexAlbedo;
uniform sampler2D TexShadow[ShadowMapCount];

uniform MaterialData{
	vec4 Color;
	float Metallic;
	float Roughness;
	float AO; // ambient occlusion
	float Padding;
}Material;

#ifdef NORMAL_MAPPING 
uniform sampler2D TexNormal;
in mat3 TBN;
#endif 

#ifdef VERTEX_COLORS
in vec3 Color;
#endif

out vec4 FragColor;

// computes ratio between specular and diffuse reflection, or how much the surface reflects lights versus how much it refracts light 
// F0 constant 0.04 for dielectric materials, see tabels for metallic materials
vec3 fresnelSchlick(float CosTheta, vec3 F0){
	return F0 + (1.0 - F0) * pow(max(1.0 - CosTheta, 0.0), 5.0);
}//fresenelSchlick 

float DistributionGGX(vec3 N, vec3 H, float Roughness){
	float a = Roughness*Roughness;
	float a2 = a*a;
	float NdotH = max(dot(N,H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float Num = a2;
	float Denom = (NdotH2 * (a2 - 1.0) + 1.0);
	Denom = PI * Denom * Denom;
	return Num/Denom;
}//DistributionGGX 

float GeometrySchlickGGX(float NdotV, float Roughness){
	float r = (Roughness + 1.0);
	float k = (r*r)/8.0;
	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return num/denom;
}//GeometrySchlickGGX

float GeometrySmith(vec3 N, vec3 V, vec3 L, float Roughness){
	float NdotV = max(dot(N,V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, Roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, Roughness);
	return ggx1 * ggx2;
}//GeometrySmith 

#ifdef DIRECTIONAL_LIGHTS
#ifdef MULTIPLE_SHADOWS
float shadowCalculationDirectionalLight(vec3 FragPosWorldSpace, vec3 Normal, vec3 LightDir, uint LightIndex){
	float Rval = 0.0f; // no shadow
	int ShadowIndex = DirLights.ShadowIDs[LightIndex].x;
	if(ShadowIndex != -1){
		float bias = max(10.0*ShadowBias * (1.0 - dot(Normal, LightDir)), ShadowBias);

		vec4 FragPosLightSpace = (DirLights.LightSpaceMatrices[LightIndex] * vec4(FragPosWorldSpace, 1.0));
		FragPosLightSpace /= FragPosLightSpace.w;

		vec3 ProjCoords = FragPosLightSpace.xyz * vec3(0.5) + vec3(0.5);  // mapping [-1,1] -> [0,1]	
		float CurrentDepth = ProjCoords.z;

		#ifdef PCF_SHADOWS
		// soft shadows 
		vec2 TexelSize = 1.0/vec2(textureSize(TexShadow[ShadowIndex], 0));
		
		for(int x = -PCFFilterSize; x <= PCFFilterSize; ++x){
			for(int y = -PCFFilterSize; y <= PCFFilterSize; ++y){
				float pcfDepth = texture(TexShadow[ShadowIndex], ProjCoords.xy + vec2(x,y) * TexelSize).r;
				Rval += (CurrentDepth - bias > pcfDepth) ? 1.0 : 0.0;
			}
		}
		Rval /= float((PCFFilterSize*2 +1) * (PCFFilterSize*2 + 1));
		#else
		// simple computations 
		float ClosestDepth = texture(TexShadow[ShadowIndex], ProjCoords.xy).r;
		Rval = (CurrentDepth - bias > ClosestDepth) ? 1.0 : 0.0;
		#endif
	}
	return Rval;
}//shadowCalculationDirectionalLight
#else
// only allow one shadow for older devices
float shadowCalculationDirectionalLight(vec3 FragPosWorldSpace, vec3 Normal, vec3 LightDir, uint LightIndex){
	float Rval = 0.0f; // no shadow
	int ShadowIndex = DirLights.ShadowIDs[LightIndex].x;
	if(ShadowIndex != -1){
		float bias = max(10.0*ShadowBias * (1.0 - dot(Normal, LightDir)), ShadowBias);

		vec4 FragPosLightSpace = (DirLights.LightSpaceMatrices[LightIndex] * vec4(FragPosWorldSpace, 1.0));
		FragPosLightSpace /= FragPosLightSpace.w;

		vec3 ProjCoords = FragPosLightSpace.xyz * vec3(0.5) + vec3(0.5);  // mapping [-1,1] -> [0,1]	
		float CurrentDepth = ProjCoords.z;

		#ifdef PCF_SHADOWS
		// soft shadows 
		vec2 TexelSize = 1.0/vec2(textureSize(TexShadow[0], 0));
		
		for(int x = -PCFFilterSize; x <= PCFFilterSize; ++x){
			for(int y = -PCFFilterSize; y <= PCFFilterSize; ++y){
				float pcfDepth = texture(TexShadow[0], ProjCoords.xy + vec2(x,y) * TexelSize).r;
				Rval += (CurrentDepth - bias > pcfDepth) ? 1.0 : 0.0;
			}
		}
		Rval /= float((PCFFilterSize*2 +1) * (PCFFilterSize*2 + 1));
		#else
		// simple computations 
		float ClosestDepth = texture(TexShadow[0], ProjCoords.xy).r;
		Rval = (CurrentDepth - bias > ClosestDepth) ? 1.0 : 0.0;
		#endif
	}
	return Rval;
}//shadowCalculationDirectionalLight
#endif
#endif


vec3 adjustContrast(vec3 Color, float Value){
	return 0.5 + Value * (Color - 0.5);
}//adjustContrast

// Value in %
vec3 adjustSaturation(vec3 Color, float Value){
	const vec3 LuminosityFactor = vec3(0.2126, 0.7152, 0.0722);
	vec3 Grayscale = vec3(dot(Color, LuminosityFactor));
	return mix(Grayscale, Color, Value);
}//adjustSaturation

vec3 adjustBrightness(vec3 Color, float Value){
	return Value * Color;
}//adjustBrightness 

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

vec3 cookTorranceBRDF(vec3 V, vec3 N, vec3 H, vec3 L, vec3 Radiance, vec3 F0, vec3 Albedo, float Roughness, float Metallic){
	float NDF = DistributionGGX(N, H, Roughness);
	float G = GeometrySmith(N, V, L, Roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - Metallic;

	vec3 Numerator = NDF * G * F;
	float Denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N,L), 0.0) + 0.001;
	vec3 Specular = Numerator / Denominator;

	// compute outging radiance L0 
	float NdotL = max(dot(N, L), 0.0);
	return (kD * Albedo / PI + Specular) * Radiance * NdotL;
}//cookTorranceBRDF

void main(){
	float Roughness = Material.Roughness; // texture(TexNormal, UV).w;
	float Metallic = Material.Metallic; // texture(TexAlbedo, UV).w;
	float Ao = Material.AO; //0.0; //texture(TexDepth, UV).w;

	vec4 TexColor = texture(TexAlbedo, UV);

	float Alpha = TexColor.a * Material.Color.a;

	// and the diffuse per-fragment color 
	if(Alpha < 0.01) discard;
	
	#ifdef VERTEX_COLORS
	vec3 Albedo = Color * Material.Color.rgb * TexColor.rgb;
	#else
	vec3 Albedo = Material.Color.rgb * TexColor.rgb;
	#endif
	Albedo = pow(Albedo, vec3(Gamma));

	// compute normals 
	#ifdef NORMAL_MAPPING 
	vec3 Norm = texture(TexNormal, UV).rgb * 2.0 - 1.0;
	vec3 Normal = normalize(TBN * Norm);
	#else
	vec3 Normal = normalize(N);
	#endif

	vec3 CameraPos = Camera.Position.xyz;

	vec3 WorldPos = Pos;
	vec3 V = normalize(CameraPos - WorldPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, Albedo.xyz, Metallic);

	vec3 Lo = vec3(0.0);

	#ifdef DIRECTIONAL_LIGHTS
	// compute directional lights contribution
	for(uint i=0U; i < DirLightCount; ++i){
		// calculate per-light radiance
		vec3 L = normalize(-DirLights.Directions[i].xyz);
		vec3 H = normalize(V + L);
		float Shadow = shadowCalculationDirectionalLight(WorldPos, Normal, L, i);
		vec3 Radiance = DirLights.Colors[i].w * DirLights.Colors[i].xyz; // color * intensity
		Lo += (1.0 - Shadow) * cookTorranceBRDF(V, Normal, H, L, Radiance, F0, Albedo, Roughness, Metallic);
	}//for[all light sources]
	#endif

	#ifdef POINT_LIGHTS
	// compute point lights contribution
	for(uint i=0U; i < PointLightCount; ++i){
		vec3 L = PointLights.Position[i].xyz - WorldPos;
		float Distance = length(L);

		vec3 Atten = PointLights.Attenuation[i].xyz; // linear, constant, quadratic attenuation 
		float Attenuation = 1.0 / max(1.0, (Atten.x + Atten.y * Distance + Atten.z * (Distance*Distance)));

		if(Attenuation > 0.01){
			L = normalize(L);
			vec3 H = normalize(V + L);
			float Shadow = 0.0;
			vec3 Radiance = Attenuation * PointLights.Color[i].w * PointLights.Color[i].xyz;
			Lo += (1.0 - Shadow) * cookTorranceBRDF(V,Normal,H,L, Radiance, F0, Albedo, Roughness, Metallic);
		}
	}//for[point lights]
	#endif

	#ifdef SPOT_LIGHTS
	// compute spot lights contribution
	for(uint i=0U; i < SpotLightCount; ++i){
		vec3 L = SpotLights.Position[i].xyz - WorldPos;
		float Distance = length(L);
		vec3 Atten = SpotLights.Attenuation[i].xyz;
		float Attenuation = 1.0 / max(1.0, (Atten.x + Atten.y*Distance + Atten.z*Distance*Distance));

		L = normalize(L);
		vec3 H = normalize(V + L);
		float Shadow = 0.0;
		
		float InnerCutOff = SpotLights.Attenuation[i].w;
		float OuterCutOff = SpotLights.Direction[i].w;
		float Theta = dot(L, -SpotLights.Direction[i].xyz);
		if(Theta > OuterCutOff && Attenuation > 0.01){
			float Epsilon = InnerCutOff- OuterCutOff;
			float Damping = clamp((Theta-OuterCutOff) / Epsilon, 0.0, 1.0);

			vec3 Radiance = Damping * Attenuation * SpotLights.Color[i].w * SpotLights.Color[i].xyz;
			Lo += (1.0 - Shadow) * cookTorranceBRDF(V,Normal,H,L, Radiance, F0, Albedo, Roughness, Metallic);
		}

	}//for[spot lights]
	#endif

	vec3 Ambient = vec3(0.1) * Albedo * 1.0;
	vec3 Col = Ambient + /*(1.0 - Ao)**/ Lo;

	// Tone Mapping (Reinhardt operator)
	Col = vec3(1.0) - exp(-Col * Exposure);
	Col = pow(Col, vec3(1.0/Gamma));

	Col = adjustColorAttributes(Col, Saturation, Brightness, Contrast);
		
	FragColor = vec4(Col, Alpha);
}//main