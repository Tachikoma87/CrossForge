#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D HeightMap;

void main(){
    float height = texture(HeightMap, TexCoords).x;

    if (height > 0.5) {
        FragColor.g = (height - 0.5) * 2;
    }
    else {
        FragColor.b = 1 - height * 2;
    }

    FragColor.a = 1.0;
}
