#version 430 core

layout (location = 0) in vec3 aPos;

out vec2 UV;

void main() {
    UV = aPos.xy / 2 + 0.5;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
