#version 330 core

layout (std140) uniform CameraData{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec4 Position;
} Camera;

layout(std140) uniform ModelData{
    mat4 ModelMatrix;
};

layout(location = 0) in vec2 Position;
out float Height;

uniform sampler2D TexAlbedo;

void main(){
    vec4 world_position = ModelMatrix * vec4(Position.x, 0.0, Position.y, 1.0);

    float scale = 32; // 2 * level
    // scale = world_position.y;

    vec2 snapped_pos = round(Camera.Position.xz / scale) * scale;
    world_position.xz += snapped_pos;

    vec2 sample_pos = world_position.xz / vec2(2048, 2048) + vec2(0.5, 0.5);
    vec4 height = texture(TexAlbedo, sample_pos);

    world_position.y = height.x * 100;

    Height = height.x;

    gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * world_position;
}
