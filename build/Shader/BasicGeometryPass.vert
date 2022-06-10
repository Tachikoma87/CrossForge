#version 330 core


#ifdef SKELETAL_ANIMATION
const uint BoneCount = 40U;

layout (std140) uniform BoneData{
	mat4 SkinningMatrix[BoneCount];
}Bones;
#endif

layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(std140) uniform ModelData{
	mat4 ModelMatrix;
};//ModelData

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec3 UVW;

#ifdef SKELETAL_ANIMATION
layout (location = 4) in ivec4 BoneIndices;
layout (location = 5) in vec4 BoneWeights;
#endif

out vec3 Pos;
out vec3 N;
out vec2 UV;


void main(){
	vec4 Po = vec4(Position, 1.0);
	vec4 No = vec4(Normal, 0.0);

#ifdef SKELETAL_ANIMATION 
	mat4 T = mat4(0);
	for(uint i = 0U; i < 4U; ++i){
		T += BoneWeights[i] * Bones.SkinningMatrix[BoneIndices[i]];	
	}//for[4 weights]
	Po = T * vec4(Position, 1.0);
	No = transpose(inverse(T)) * vec4(Normal, 0.0);
#endif 

	N = mat3(transpose(inverse(ModelMatrix))) * No.xyz;
	Pos = (ModelMatrix * Po).xyz;
	UV = UVW.xy;
	//gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * ModelMatrix * vec4(Position, 1.0);
	gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * ModelMatrix * Po;
}//main