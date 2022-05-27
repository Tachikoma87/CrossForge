#version 330 core 

out vec4 gAlbedoSpec;

in vec3 Pos;
in vec3 CameraPos;

void main(){

	// COLOR ---------------------------------------------------------
	gAlbedoSpec= vec4(0, 0, 1, 0.7);
}
