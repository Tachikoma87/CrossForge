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
layout (location = 1) in vec2 UV;

layout(rgba32f, binding = 0) uniform image2D textureDisplacement;

out vec3 N;

void main(){
vec3 displacement = imageLoad(textureDisplacement, ivec2(UV * imageSize(textureDisplacement))).xyz;
    N = vec3(clamp(displacement.y, 0, 1), displacement.y, 0);
    

	gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(Position + vec3(imageLoad(textureDisplacement, ivec2(UV * imageSize(textureDisplacement))).xyz * 2), 1);
}




