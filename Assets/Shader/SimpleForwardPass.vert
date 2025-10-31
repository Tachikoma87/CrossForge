#version 330 core



layout(std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(std140) uniform ModelData{
	mat4 ModelMatrix;
	mat4x3 NormalMatrix;
}Model;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 3) in vec3 UVW;


out vec3 Pos;
out vec3 N;
out vec2 UV;


void main(){

	vec4 Po = vec4(Position, 1.0);
	vec4 No = vec4(Normal, 0.0);

	N = Model.NormalMatrix * No; // normalization in fragment shader

	Pos = (Model.ModelMatrix * Po).xyz;
	UV = UVW.xy;

	gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * Model.ModelMatrix * Po;
	
}//main