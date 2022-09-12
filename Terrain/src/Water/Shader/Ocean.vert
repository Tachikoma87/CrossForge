#version 430 core

layout(std140) uniform CameraData {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec4 Position;
} Camera;

layout(std140) uniform ModelData {
    mat4 ModelMatrix;
};

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 instanceOffset;

uniform sampler2D heightMapTexture;
uniform sampler2D shoreDistTexture;
uniform sampler2D displacementTexture;

uniform float amplitudeScale;
uniform float choppyness;
uniform float widthScale;
uniform float time;
uniform vec2 windDirection;
uniform float uvScale;
uniform bool lowQuality;



out vec2 UVcord;
out vec3 POS;
out vec3 CAM;
out float newAmplitudeScale;

float getShoreWaveFactor(vec2 pos) {
	float maxHeight = 0.5;
	float minHeight = 0.3;

	vec2 tSize = textureSize(heightMapTexture, 0);
	vec2 uvSampleCord = pos;
	uvSampleCord += tSize / 2;
	uvSampleCord = uvSampleCord / tSize;
	uvSampleCord = clamp(uvSampleCord, 0, 1);

	
	float ret = 1 - texture(shoreDistTexture, uvSampleCord).x;


	float ret2 = texture(heightMapTexture, uvSampleCord).x;
	float dist = length(pos / tSize);
	ret2 = (ret2 - minHeight) / (maxHeight - minHeight);
	ret2 -= clamp((dist * 2 - 0.8) * 3, 0, 1);
	clamp(ret2, 0, 1);

	return mix(ret, ret2, 0.75);
}

float shoreWave(float factor) {
	float waveLength = 2.0;
	float waveSpeed = 0.3;
	float offset = waveLength * 0.05;
	/*
	float a = mod(-time * waveSpeed + factor, waveLength) / waveLength - 1 / 1.5;
	a = clamp(a, 0, 1);

	float b = -mod(-time * waveSpeed + factor, waveLength) / ((lowQuality ? 0.4 : 0.4) * waveLength) + 0.5 / 1.5;
	b = clamp(b, 0, 1);
	*/
	float c = -abs(sin((-time * waveSpeed + factor + offset) * 3.1415 / waveLength)) + 0.25;

	return c * (lowQuality ? 2.5 : 10);//clamp((a + b) * factor * 2, 0, 1);
}


void main(){
	float circleRadius = lowQuality ? 300 : 1200;

	POS = Camera.Position.xyz * vec3(1, 0, 1);
	POS.x = int(POS.x / widthScale) * widthScale;
	POS.z = int(POS.z / widthScale) * widthScale;

    UVcord = uv + POS.xz / uvScale;// + windDirection * time;


    float dist = length(Position + instanceOffset);
 
   

    float shoreFactor = getShoreWaveFactor(Position.xz + instanceOffset.xz + POS.xz) * 2;
    float waveFactor = shoreWave(shoreFactor);
    shoreFactor = clamp(shoreFactor, 0.0, 0.3);

    newAmplitudeScale = amplitudeScale * (1 - shoreFactor);
    vec3 displacement = dist > circleRadius ? vec3(0) : texture(displacementTexture, UVcord).xyz * newAmplitudeScale + waveFactor * shoreFactor;// imageLoad(textureDisplacement, ivec2(mod(UVcord, 1) * imageSize(textureDisplacement))).xyz * newAmplitudeScale + waveFactor * shoreFactor;
	

    //vec4 derivatives = imageLoad(textureNormal, ivec2(mod(UVcord, 1) * imageSize(textureNormal)));
	

    POS = POS + Position + instanceOffset + displacement * vec3(choppyness, 1, choppyness);
    CAM = Camera.Position.xyz;

    vec4 ret = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(POS, 1);
	gl_Position = ret;
}
