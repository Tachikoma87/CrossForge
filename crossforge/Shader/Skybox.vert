#version 330 core

layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(std140) uniform ModelData{
	mat4 ModelMatrix;
	mat4x3 NormalMatrix;
}Model;

layout(location = 0) in vec3 Position;

out vec3 TexCoords;

void main(){

	TexCoords = Position;

	mat4 Cam = mat4(mat3(Camera.ViewMatrix));

	vec4 Pos = Camera.ProjectionMatrix * Cam * Model.ModelMatrix * vec4(Position, 1.0);
	gl_Position = Pos.xyww;
}//main 

