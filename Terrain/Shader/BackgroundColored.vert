#version 330 core
layout(location = 0) in vec2 Position;
layout(location = 3) in vec2 UV;
out vec2 TexCoords;

uniform vec4 color;
uniform mat4 projection;

void main(){
	gl_Position = vec4(clamp(Position.x, -1, 1), clamp(Position.y, -1, 1), 0, 1);//projection * vec4(Position, 0.0, 1.0);
	TexCoords = UV;
}//main
