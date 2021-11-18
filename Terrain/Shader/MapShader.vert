#version 330 core

layout(std140) uniform CameraData {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec4 Position;
} Camera;

layout(std140) uniform ModelData {
    mat4 ModelMatrix;
};

uniform sampler2D HeightMap;
uniform float MapScale;
uniform float MapHeight;

in vec2 VertPosition;

out vec3 FragPosition;
out vec2 SamplePosition;
out float Height; // normalized

void main(){
    vec4 worldPosition = ModelMatrix * vec4(VertPosition.x, 0.0, VertPosition.y, 1.0);

    vec2 mapSize = textureSize(HeightMap, 0);

    // first 0.5 centeres the texel position and the second centers the map texture
    SamplePosition = (worldPosition.xz + 0.5) / mapSize;
    SamplePosition = vec2(SamplePosition.x + 0.5, -SamplePosition.y + 0.5);
    Height = texelFetch(HeightMap, ivec2(SamplePosition * mapSize), 0).x;

    worldPosition.y = Height * MapHeight;
    worldPosition.xyz *= MapScale;
    FragPosition = worldPosition.xyz;

    gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * worldPosition;

}
