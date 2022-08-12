#version 420 core

layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(binding=0) uniform sampler2D texColor;
layout(binding=1) uniform sampler2D texPosition;
layout(binding=2) uniform sampler2D texNormal;

uniform float uTime;
uniform ivec2 uResolution;
const bool globalAO = true;
const bool randAO = false;

in vec2 TexCoords;
out vec4 FragColor;

/////////////////////////////////////////////////////////////////////////////////////// SSAO begin
// https://www.shadertoy.com/view/WtyfR1
// SSAO (Screen Space AO) - by moranzcw - 2021
// Email: moranzcw@gmail.com
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
#define PI 3.14159265359
#define AOradius 0.005
#define Samples 64.0
// --------------------------------------
// oldschool rand() from Visual Studio
// --------------------------------------
int   seed = 1;
void  srand(int s ) { seed = s; }
int   rand(void)  { seed=seed*0x343fd+0x269ec3; return (seed>>16)&32767; }
float frand(void) { return float(rand())/32767.0; }
// --------------------------------------
// hash by Hugo Elias
// --------------------------------------
int hash( int n ) { n=(n<<13)^n; return n*(n*n*15731+789221)+1376312589; }
vec3 sphereVolumeRandPoint()
{
	vec3 p = vec3(frand(),frand(),frand()) * 2.0 - 1.0;
	while(length(p)>1.0)
	{
		p = vec3(frand(),frand(),frand()) * 2.0 - 1.0;
	}
	return p;
}
float depth(vec2 coord)
{
	vec2 uv = coord;
	return min(1.0,length(texture(texPosition, uv).xyz - Camera.Position.xyz)/1000.0);
}
float SSAO(vec2 coord)
{
	float cd = depth(coord);
	float screenRadius = 0.5 * (AOradius / cd) * (16.0/9.0);
	float li = 0.0;
	float count = 0.0;
	for(float i=0.0; i<Samples; i++)
	{
		vec3 p = sphereVolumeRandPoint() * frand();
		vec2 sp = vec2(coord.x + p.x * screenRadius, coord.y + p.y * screenRadius);
		float d = depth(sp);
		float at = pow(length(p)-1.0, 2.0);
		li += step(cd + p.z * AOradius, d) * at;
		count += at;
	}
	return max(0.0,min(1.0,(li / count)+0.5));
}
/////////////////////////////////////////////////////////////////////////////////////// SSAO end

void main(){
	//SSAO
	float ao = 1.0;
//	if (!globalAO) {
//		if (randAO) {
//			ivec2 q = ivec2(TexCoords*vec2(1920,1080));
//			int iFrame = int(uTime*144);			
//			srand( hash(q.x+hash(q.y+hash(1117*(iFrame)))));
//		}
//		if (distScene < waterDist && abs(geoWorldPos.x) > 0.001)
//			ao = max(SSAO(TexCoords),clamp((-geoWorldPos.y*0.1+1.25),0.0,1.0));
//		else
//			ao = 1.0;
//	} else 
		ao = SSAO(TexCoords);
	ao = pow(ao,2.0)*1.0;
	
	
	FragColor = vec4(texture(texColor, TexCoords).xyz*ao,0.0);
}