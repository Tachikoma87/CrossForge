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
uniform sampler2D TexNormal;


in vec3 Pos;
in vec3 N;
in vec2 UV;


void main(){
	// store the framgent position vector in the first gBuffer texture 
	gPosition = vec4(Pos, Material.AO);
	// also store the per-fragment normals into the gBuffer 
	gNormal = vec4(normalize(N + (texture(TexNormal, UV) * 2 - 1).rgb), Material.Roughness);
	// and the diffuse per-fragment color 
	gAlbedoSpec.rgb = Material.Color.rgb * texture(TexAlbedo, UV).rgb;
	// store the specular intensity in gAlbedoSpec s alpha component 
	gAlbedoSpec.a = Material.Metallic;
}//main 