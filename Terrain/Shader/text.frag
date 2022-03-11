#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D glyphMap;
uniform vec3 textColor;

void main()
{
//     vec4 sampled = vec4(1.0, 1.0, 1.0, texture(glyphMap, TexCoords).r);
//     FragColor = vec4(textColor, 1.0) * sampled;
//     FragColor = vec4(textColor, texture(glyphMap, TexCoords).r);
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
