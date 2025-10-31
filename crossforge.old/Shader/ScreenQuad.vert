#version 330 core 

//#version 300 es
//precision lowp float;
layout(std140) uniform ModelData{
	mat4 ModelMatrix;
	mat4x3 NormalMatrix;
}Model; 

layout(location = 0) in vec2 Position;
layout(location = 3) in vec2 UVW;

out vec2 TexCoords;

void main(){

	gl_Position = Model.ModelMatrix * vec4(Position.x, Position.y, 0.0, 1.0);
	TexCoords = UVW;
}//main 