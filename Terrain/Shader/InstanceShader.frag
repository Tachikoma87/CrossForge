#version 330 core 

layout(location = 0) out vec4 FragmentColor;

layout (std140) uniform MaterialData{
	vec4 Color;
	float Metallic;
	float Roughness;
	float AO; // ambient occlusion
	float Padding;
}Material;

uniform sampler2D TexAlbedo;
uniform sampler2D TexDepth;

in vec3 Pos;
in vec3 N;
in vec2 UV;
in vec3 test;

void main(){
	if (texture(TexDepth, UV).r < 0.5) {
		discard;
	}

	FragmentColor = texture(TexAlbedo, UV);
}