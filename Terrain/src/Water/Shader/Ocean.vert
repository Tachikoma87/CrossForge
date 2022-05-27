#version 420 core

layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(std140) uniform ModelData{
	mat4 ModelMatrix;
};

layout (location = 0) in vec3 Position;


out vec3 Pos;
out vec3 CameraPos;

void main(){
	Pos = mat3(ModelMatrix) * Position;
	CameraPos = Camera.Position.xyz;


	gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * ModelMatrix * vec4(Position , 1);
}




