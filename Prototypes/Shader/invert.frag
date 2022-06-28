#version 420 core

layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(binding=0) uniform sampler2D texColor;
layout(binding=1) uniform sampler2D texPosition;
layout(binding=2) uniform sampler2D texNormal;

uniform float mapHeight;
uniform float uTime;

in vec2 TexCoords;
out vec4 FragColor;

void main(){
	
//	FragColor = vec4(vec3(texture(texColor, TexCoords)), 1.0);
//	return;
	vec4 WorldPos = texture(texPosition, TexCoords)-vec4(0.0,mapHeight,0.0,0.0);
	
	//FragColor = WorldPos;
	//return;
	//if (length(vec4(Camera.Position-texture(texPosition, TexCoords)).xyz) > 100.0)
//	if (WorldPos.y > sin(uTime)*mapHeight*0.5)
//		FragColor = WorldPos; //vec4(1.0-vec3(texture(texColor, TexCoords)), 1.0);
//	else
//		FragColor = vec4(vec3(texture(texColor, TexCoords)), 1.0);
	if (TexCoords.x < 0.25)
		FragColor = vec4(vec3(texture(texColor, TexCoords)), 1.0);
	else if (TexCoords.x < 0.5)
		FragColor = vec4(1.0-vec3(texture(texColor, TexCoords)), 1.0);
	else if (TexCoords.x < 0.75)
		FragColor = vec4(vec3(texture(texPosition, TexCoords)), 1.0);
	else if (TexCoords.x < 1.0)
		FragColor = vec4(vec3(texture(texNormal, TexCoords)), 1.0);
	//FragColor = Camera.Position;
	else
		discard;
}
