#version 330 core

const uint MAX_VALUE = 65535u;

in vec2 TexCoords;

out vec4 FragColor;

uniform usampler2D HeightMap;

void main(){
    float height = texture(HeightMap, TexCoords).x / float(MAX_VALUE);

    if (height > 0.5) {
        FragColor.g = (height - 0.5) * 2;
    }
    else {
        FragColor.b = 1 - height * 2;
    }

    FragColor.a = 1.0;
}
