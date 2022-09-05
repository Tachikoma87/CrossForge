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



out vec3 POS;
out vec3 CAM;




void main(){
    POS = Position;
    CAM = Camera.Position.xyz;

    vec4 ret = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(POS, 1);

	gl_Position = ret;
}
