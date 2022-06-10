#version 330 core 

//#version 300 es
//precision lowp float;

layout(location = 0) in vec2 Position;
layout(location = 3) in vec2 UVW;

out vec2 UV;

void main(){
	gl_Position = vec4(Position.x, Position.y, 0.0, 1.0);
	UV = UVW;
}//main 