#version 330 core

layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(std140) uniform ModelData{
	mat4 ModelMatrix;
};

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec3 UVW;

out vec3 Pos;
out vec3 N;
out vec3 B;
out vec3 T;
out vec2 UV;
out vec3 CameraPos;

void main(){
	// Normal, Tangent, BiTangent, Position, CameraPosition in WorldSpace
	N = normalize(mat3(ModelMatrix) * Normal);
	T = normalize(mat3(ModelMatrix) * Tangent);
	B = normalize(cross(N, T));
	Pos = mat3(ModelMatrix) * Position;
	CameraPos = Camera.Position.xyz;

	UV = UVW.xy;
	gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * ModelMatrix * vec4(Position, 1);
}