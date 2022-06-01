#version 330 core
layout(location = 0) in vec2 Position;
layout(location = 3) in vec2 UV;
out vec2 TexCoords;

uniform vec4 color;
uniform mat4 projection;

void main(){
	gl_Position = projection * vec4(Position, 0.9, 1.0);
	TexCoords = UV;
}//main
