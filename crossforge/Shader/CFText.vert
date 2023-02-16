#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

layout(std140) uniform TextData{
    vec4 Color;
    vec4 SizeAndPosition; // Canvas Size and Text Position 
}Text;

void main()
{
    TexCoords = vertex.zw;

    // compute final position in window/viewport coordinates
    float XPos = vertex.x + Text.SizeAndPosition.z;
    float YPos = vertex.y + Text.SizeAndPosition.y - 1.0 - Text.SizeAndPosition.w;

    // projection to [-1,1]
    gl_Position.x = (2.0 * XPos)/Text.SizeAndPosition.x - 1.0;
    gl_Position.y = (2.0 * YPos)/Text.SizeAndPosition.y - 1.0;
    gl_Position.z = 0.0;
    gl_Position.w = 1.0;
}
