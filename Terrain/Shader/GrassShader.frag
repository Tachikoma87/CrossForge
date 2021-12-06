#version 330 core 

//#version 300 es
//precision lowp float;

// gBuffer stuff
layout(location = 0) out vec4 FragmentColor;

layout (std140) uniform MaterialData{
	vec4 Color;
	float Metallic;
	float Roughness;
	float AO; // ambient occlusion
	float Padding;
}Material;

uniform sampler2D TexAlbedo;


in vec3 Pos;
in vec3 N;
in vec2 UV;


void main(){
	FragmentColor = texture(TexAlbedo, UV);	
	
}//main 