#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D GlyphMap;


layout(std140) uniform TextData{
    vec4 Color;
    vec4 SizeAndPosition; // Canvas Size and Text Position
}Text;

void main()
{
    FragColor = vec4(Text.Color.rgb, texture(GlyphMap, TexCoords).r * Text.Color.a);
}
