#version 300 es 		
in vec4 Position; 

uniform float XOffset;

layout(std140) uniform ModelData2{
	mat4 Matrix;
};

void main(void){ 
	gl_Position = max(Matrix[0][0], 1.0f) * Position + vec4(XOffset, 0.0, 0.0, 0.0); 
}