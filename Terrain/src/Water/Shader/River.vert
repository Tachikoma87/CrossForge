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
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in float Width;

uniform vec2 nearFarPlane;
uniform float time;
uniform vec2 windDirection;

out vec3 POS;
out vec3 CAM;
out vec3 SCREENUV;
out vec2 UV;
out float WIDTH;
out vec3 NORMAL;
out mat3 TANGENTSPACE;


void main(){
    POS = Position;
    CAM = Camera.Position.xyz;
    UV = uv;
    NORMAL = Normal;
    TANGENTSPACE = mat3(Tangent, Normal, cross(Tangent, Normal));
    WIDTH = Width;

    vec4 ret = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(POS, 1);
    SCREENUV = ret.xyz / ret.w / 2 + 0.5;
	gl_Position = ret;
}
