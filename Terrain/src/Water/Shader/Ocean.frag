#version 330 core 

out vec4 gAlbedoSpec;

in vec3 N;

void main(){

	// COLOR ---------------------------------------------------------
	gAlbedoSpec= vec4(N.x, N.x, 1 + N.y, 1);
}
