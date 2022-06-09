#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aStrand;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aColor;

out vec3 P_world;
flat out uint strand;
out vec3 T_world;
out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 positionWS = model * vec4(aPos, 1.0);
	P_world = positionWS.xyz;
	vec4 tangentWS = model * vec4(aTangent, 0.0);
	T_world = tangentWS.xyz;
	strand = aStrand;
	color = aColor;

	gl_Position = projection * view * positionWS;
}