#version 330 core 

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube Skybox;

layout (std140) uniform ColorAdjustmentData{
	float Saturation;
	float Contrast;
	float Brightness;
	float Padding;
}ColorAdjustment;

vec3 adjustColorAttributes(vec3 Color){

	// adjust brightness 
	vec3 Rval = vec3(ColorAdjustment.Brightness) * Color;
	// adjust Contrast
	Rval = 0.5 + ColorAdjustment.Contrast * (Rval - vec3(0.5));
	// adjust Saturation 
	const vec3 LuminosityFactor = vec3(0.2126, 0.7152, 0.0722);
	vec3 Grayscale = vec3(dot(Rval, LuminosityFactor));
	Rval = mix(Grayscale, Rval, ColorAdjustment.Saturation);
	return Rval;
}//ajdustColorAttributes


void main(){
	FragColor = texture(Skybox, TexCoords);
	FragColor.xyz = adjustColorAttributes(FragColor.xyz);
}//main