#version 330 core

in float Height;
out vec4 FragColor;

void main(){
    FragColor = vec4(0.0, 0.0, Height * 10 + 2, 1.0);
    // FragColor = vec4(0.0, 0.0, 10, 10.0);
}
