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

layout(rgba32f, binding = 0) uniform image2D textureDisplacement;
layout(rgba32f, binding = 1) uniform image2D textureNormal;

uniform float amplitudeScale;
uniform float choppyness;
uniform float widthScale;
uniform float time;
uniform vec2 windDirection;

out vec2 UV;
out vec3 POS;
out vec3 CAM;
out vec3 SCREENUV;
out float newAmplitudeScale;


void main(){
    UV = uv;// + windDirection * time;


    float dist = length(Position + instanceOffset);
    float outerRing = 600;
    float innerRing = 300;
    float changeFactor = 10;

    if (dist > outerRing) { 
        newAmplitudeScale = amplitudeScale;
    }
    else if (dist > innerRing){
        newAmplitudeScale = mix(1.0 / changeFactor, 1.0, clamp((dist - innerRing) / (outerRing - innerRing), 0, 1)) * amplitudeScale;
    }
    else {
        newAmplitudeScale = amplitudeScale / changeFactor;
    }
    
    vec3 displacement = imageLoad(textureDisplacement, ivec2(mod(UV, 1) * imageSize(textureDisplacement))).xyz * newAmplitudeScale;
    vec4 derivatives = imageLoad(textureNormal, ivec2(mod(UV, 1) * imageSize(textureNormal)));

    POS = Position + instanceOffset + displacement * vec3(choppyness, 1, choppyness);
    CAM = Camera.Position.xyz;

    vec4 ret = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(POS, 1);
    SCREENUV = ret.xyz / ret.w / 2 + 0.5;
	gl_Position = ret;
}
