#version 330 core 

//#version 300 es
//precision lowp float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexAlbedo;

void main(){
	FragColor = texture(TexAlbedo, TexCoords);
	FragColor.a = 1.0;
}//main 