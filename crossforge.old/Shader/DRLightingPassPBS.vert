#version 330 core 

layout (location = 0) in vec2 Position;
layout (location = 3) in vec2 UVW;

layout(std140) uniform ModelData{
	mat4 ModelMatrix;
	mat4x3 NormalMatrix;
}Model;//ModelData

out vec2 UV;

void main(){
	gl_Position = Model.ModelMatrix * vec4(Position.x, Position.y, 0.0, 1.0);
	UV = UVW; 
}//main 