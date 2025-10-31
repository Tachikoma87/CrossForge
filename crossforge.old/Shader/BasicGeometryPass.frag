#version 330 core 

//#version 300 es
//precision lowp float;

// gBuffer stuff
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

layout (std140) uniform MaterialData{
	vec4 Color;
	float Metallic;
	float Roughness;
	float AO; // ambient occlusion
	float Padding;
}Material;

uniform sampler2D TexAlbedo;

#ifdef NORMAL_MAPPING 
uniform sampler2D TexNormal;
in mat3 TBN;
#endif 

#ifdef VERTEX_COLORS
in vec3 Color;
#endif

in vec3 Pos;
in vec3 N;
in vec2 UV;

void main(){
	
	vec4 TexColor = texture(TexAlbedo, UV);
	float Alpha = TexColor.a * Material.Color.a;

	// and the diffuse per-fragment color 
	if(Alpha < 0.01) discard;
	
	#ifdef VERTEX_COLORS
	gAlbedoSpec.rgb = Color * Material.Color.rgb * TexColor.rgb;
	#else
	gAlbedoSpec.rgb = Material.Color.rgb * TexColor.rgb;
	#endif
	

	// store the framgent position vector in the first gBuffer texture 
	gPosition = vec4(Pos, Material.AO);

	// also store the per-fragment normals into the gBuffer 
	#ifdef NORMAL_MAPPING 
	vec3 normal = normalize(texture(TexNormal, UV).rgb * 2.0 - 1.0);
	gNormal = vec4(normalize(TBN * normal), Material.Roughness);
	#else
	gNormal = vec4(normalize(N), Material.Roughness);
	#endif

	// store the specular intensity in gAlbedoSpec s alpha component 
	gAlbedoSpec.a = Material.Metallic;
}//main 