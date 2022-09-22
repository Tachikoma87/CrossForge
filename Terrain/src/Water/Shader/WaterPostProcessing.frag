#version 430 core 

out vec4 gColor;

uniform sampler2D baseColorTexture;
uniform sampler2D reflectColorTexture;

in vec2 UV;


void main(){
	
	vec4 bColor = texture(baseColorTexture, UV);
	vec4 rColor = texture(reflectColorTexture, UV);

	// COLOR ---------------------------------------------------------
	if (bColor.xyz == vec3(0)) discard;

	gColor = mix(rColor, bColor, bColor.a);
	gColor = vec4(gColor.rgb, 1);
	//gColor = vec4(0, 0, 1, 1);
}
