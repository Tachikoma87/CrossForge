#version 330 core

#ifdef SKELETAL_ANIMATION
const uint BoneCount = 40U;

layout (std140) uniform BoneData{
	mat4 SkinningMatrix[BoneCount];
}Bones;
#endif

#ifdef MORPHTARGET_ANIMATION 
uniform samplerBuffer MorphTargetDataBuffer;

layout (std140) uniform MorphTargetData{
	// 0 is offset in elements (float) to next morph target
	// 1 is number of active animations 
	ivec4 Data; 
	ivec4 ActivationIDs[3];			///< Morph target IDs (capped at 12)
	vec4 ActivationStrengths[3];	///< Morph target activation strengths (capped at 12)
}MorphTargets;

#endif

layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(std140) uniform ModelData{
	mat4 ModelMatrix;
};//ModelData

#define MAX_INSTANCE_COUNT 500
//struct instanceMat {
//	mat3 instancedRotation;
//	vec3 instancedTranslation;
//};
//
//layout(std140) uniform InstancedData{
//	instanceMat instancedMat[MAX_INSTANCE_COUNT];
//};

layout(std140) uniform InstancedData{
	mat4 instancedMat[MAX_INSTANCE_COUNT];
};

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec3 UVW;

#ifdef SKELETAL_ANIMATION
layout (location = 4) in ivec4 BoneIndices;
layout (location = 5) in vec4 BoneWeights;
#endif

#ifdef VERTEX_COLORS 
layout (location = 6) in vec3 VertexColor;
out vec3 Color;
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
	No = T * vec4(No);
#endif 

#ifdef MORPHTARGET_ANIMATION
	vec3 Displ = vec3(0);
	for(int i = 0; i < MorphTargets.Data[1]; ++i){
		// compute Offset to fetch from 
		int ID = MorphTargets.ActivationIDs[i/3][i%3];
		int Offset = (ID * MorphTargets.Data[0]) + gl_VertexID;
		Displ += MorphTargets.ActivationStrengths[i/3][i%3] * texelFetch(MorphTargetDataBuffer, Offset).rgb;
	}//for[active morph targets]

	Po += vec4(Displ, 0.0);
#endif

#ifdef VERTEX_COLORS
Color = VertexColor;
#endif

//	N = instancedMat[gl_InstanceID].instancedRotation * No.xyz;
//	Pos = instancedMat[gl_InstanceID].instancedRotation * Po.xyz + instancedMat[gl_InstanceID].instancedTranslation;
//	UV = UVW.xy;
//	gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(Pos,1.0);

	N = (instancedMat[gl_InstanceID] * No).xyz;
	Pos = (instancedMat[gl_InstanceID] * Po).xyz;
	UV = UVW.xy;
	gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * instancedMat[gl_InstanceID] * Po;

//	N = (ModelMatrix * No).xyz;
//	Pos = (ModelMatrix * Po).xyz;
//	UV = UVW.xy;
//	gl_Position = Camera.ProjectionMatrix * Camera.ViewMatrix * ModelMatrix * Po;
}//main