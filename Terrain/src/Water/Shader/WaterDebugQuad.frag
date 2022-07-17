#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D Texture;

void main(){
    FragColor = vec4(texture(Texture, TexCoords));
    FragColor.a = 1.0;
}
