#version 330 core

const float MAP_HEIGHT = 1000;
const ivec3 OFFSET = ivec3(-1, 0, 1);
const int LOD_LEVELS = 4;
const float SCALE = pow(2, LOD_LEVELS);

layout(std140) uniform CameraData {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec4 Position;
} Camera;

layout(std140) uniform ModelData {
    mat4 ModelMatrix;
};

layout(location = 0) in vec2 VertPosition;
out vec3 FragPosition;
out vec2 SamplePosition;
out float Height;

uniform sampler2D HeightMap;

float getHeight(vec4 sampledHeight) {
    return sampledHeight.x * MAP_HEIGHT;
}

void main(){
    vec4 worldPosition = ModelMatrix * vec4(VertPosition.x, 0.0, VertPosition.y, 1.0);

    SamplePosition = (worldPosition.xz) / textureSize(HeightMap, 0) + 0.5; // the 0.5 centers the map texture
    Height = getHeight(texture(HeightMap, SamplePosition));

    worldPosition.y = Height;
    FragPosition = worldPosition.xyz;

    gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * worldPosition;

}
