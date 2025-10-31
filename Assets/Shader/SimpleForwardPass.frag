#version 330 core 

//#version 300 es
//precision lowp float;

out vec4 FragColor;

in vec3 Pos;
in vec3 N;
in vec2 UV;

uniform sampler2D TexAlbedo;

void main(){
	FragColor.xyz = texture2D(TexAlbedo, UV).xyz;
	FragColor.w = 1.0; // = vec4(1.0, UV.x, UV.y, 1.0);
}//main 