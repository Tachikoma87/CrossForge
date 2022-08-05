// https://www.shadertoy.com/view/Ms2SD1
/*
 * "Seascape" by Alexander Alekseev aka TDM - 2014
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * Contact: tdmaav@gmail.com
 */
#version 420 core

layout (std140) uniform CameraData{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 Position;
}Camera;

layout(binding=0) uniform sampler2D texColor;
layout(binding=1) uniform sampler2D texPosition;
layout(binding=2) uniform sampler2D texNormal;

uniform float mapHeight;
uniform float uTime;
uniform float aspectRatio;

in vec2 TexCoords;
out vec4 FragColor;

const float infinity = 1.0 / 0.0;

const int NUM_STEPS = 4;
const float PI	 	= 3.141592;
const float EPSILON	= 1e-3;
#define EPSILON_NRM 0.1/1920.0//(0.1 / iResolution.x)
//#define AA

// sea
const int ITER_GEOMETRY = 2;
const int ITER_FRAGMENT = 3;
const float SEA_HEIGHT = 0.6;
const float SEA_CHOPPY = 4.0;
const float SEA_SPEED = 0.8;
const float SEA_FREQ = 0.16;
const vec3 SEA_BASE = vec3(0.0,0.09,0.18);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6)*0.6;
#define SEA_TIME (1.0 + uTime * SEA_SPEED)
const mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);

const bool globalAO = false;
const bool randAO = false;

// math
mat3 fromEuler(vec3 ang) {
	vec2 a1 = vec2(sin(ang.x),cos(ang.x));
	vec2 a2 = vec2(sin(ang.y),cos(ang.y));
	vec2 a3 = vec2(sin(ang.z),cos(ang.z));
	mat3 m;
	m[0] = vec3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
	m[1] = vec3(-a2.y*a1.x,a1.y*a2.y,a2.x);
	m[2] = vec3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
	return m;
}
float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
	return fract(sin(h)*43758.5453123);
}
float noise( in vec2 p ) {
	vec2 i = floor( p );
	vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
	return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
					 hash( i + vec2(1.0,0.0) ), u.x),
				mix( hash( i + vec2(0.0,1.0) ), 
					 hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

// lighting
float diffuse(vec3 n,vec3 l,float p) {
	return pow(dot(n,l) * 0.4 + 0.6,p);
}
float specular(vec3 n,vec3 l,vec3 e,float s) {    
	float nrm = (s + 8.0) / (PI * 8.0);
	return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

// sky
vec3 getSkyColor(vec3 e) {
	e.y = (max(e.y,0.0)*0.8+0.2)*0.8;
	return vec3(pow(1.0-e.y,2.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4) * 1.1;
}

// sea
float sea_octave(vec2 uv, float choppy) {
	uv += noise(uv);        
	vec2 wv = 1.0-abs(sin(uv));
	vec2 swv = abs(cos(uv));    
	wv = mix(wv,swv,wv);
	return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

float map(vec3 p) {
	float freq = SEA_FREQ;
	float amp = SEA_HEIGHT;
	float choppy = SEA_CHOPPY;
	vec2 uv = p.xz; uv.x *= 0.75;
	
	float d, h = 0.0;    
	for(int i = 0; i < ITER_GEOMETRY; i++) {        
		d = sea_octave((uv+SEA_TIME)*freq,choppy);
		d += sea_octave((uv-SEA_TIME)*freq,choppy);
		h += d * amp;        
		uv *= octave_m; freq *= 1.9; amp *= 0.22;
		choppy = mix(choppy,1.0,0.2);
	}
	return p.y - h;
}

float map_detailed(vec3 p) {
	float freq = SEA_FREQ;
	float amp = SEA_HEIGHT;
	float choppy = SEA_CHOPPY;
	vec2 uv = p.xz; uv.x *= 0.75;
	
	float d, h = 0.0;    
	for(int i = 0; i < ITER_FRAGMENT; i++) {        
		d = sea_octave((uv+SEA_TIME)*freq,choppy);
		d += sea_octave((uv-SEA_TIME)*freq,choppy);
		h += d * amp;        
		uv *= octave_m; freq *= 1.9; amp *= 0.22;
		choppy = mix(choppy,1.0,0.2);
	}
	return p.y - h;
}

vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {  
	float fresnel = clamp(1.0 - dot(n,-eye), 0.0, 1.0);
	fresnel = pow(fresnel,3.0) * 0.5;
		
	vec3 reflected = getSkyColor(reflect(eye,n));    
	vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
	
	vec3 color = mix(refracted,reflected,fresnel);
	
	float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
	color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
	
	color += vec3(specular(n,l,eye,60.0));
	
	return color;
}

// tracing
vec3 getNormal(vec3 p, float eps) {
	vec3 n;
	n.y = map_detailed(p);    
	n.x = map_detailed(vec3(p.x+eps,p.y,p.z)) - n.y;
	n.z = map_detailed(vec3(p.x,p.y,p.z+eps)) - n.y;
	n.y = eps;
	return normalize(n);
}

float heightMapTracing(vec3 ori, vec3 dir, out vec3 p) {  
	float tm = 0.0;
	float tx = 10000.0;    
	float hx = map(ori + dir * tx);
	if(hx > 0.0) {
		p = ori + dir * tx;
		return tx;   
	}
	float hm = map(ori + dir * tm);    
	float tmid = 0.0;
	for(int i = 0; i < NUM_STEPS; i++) {
		tmid = mix(tm,tx, hm/(hm-hx));                   
		p = ori + dir * tmid;                   
		float hmid = map(p);
		if(hmid < 0.0) {
			tx = tmid;
			hx = hmid;
		} else {
			tm = tmid;
			hm = hmid;
		}
	}
	return tmid;
}
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

void main(){
	vec2 uv = TexCoords;
	uv.x *= aspectRatio;
	uv.x -= (aspectRatio-1.0)*0.5;
	vec2 ndc = uv * 2.0 - vec2(1.0);

	vec3 camDir = (inverse(Camera.ViewMatrix)*vec4(0.0,0.0,-1.0,0.0)).xyz;
	vec3 camUp = (inverse(Camera.ViewMatrix)*vec4(0.0,1.0,0.0,0.0)).xyz;
	
	vec3 camSide = cross(camDir, camUp);
	float focus = Camera.ProjectionMatrix[1][1];

	vec3 camPos = Camera.Position.xyz - vec3(0.0,mapHeight,0.0);
	vec3 rayDir = normalize(camSide*ndc.x + camUp*ndc.y + camDir*focus);
	
	//FragColor = raymarch(rayDir, camPos);
	vec3 geoWorldPos = texture(texPosition, TexCoords).xyz - vec3(0.0,mapHeight,0.0);
	float distScene = length(geoWorldPos - camPos);
	
	vec3 ori = camPos;
	vec3 dir = rayDir;
	// tracing
	vec3 p;
	float waterDist = heightMapTracing(ori,dir,p);
	vec3 dist = p - ori;
	vec3 n = getNormal(p, dot(dist,dist) * EPSILON_NRM);
	vec3 light = normalize(vec3(-.5,1.0,0.5));
	
	// color
	vec3 oceanCol = mix(getSkyColor(dir), getSeaColor(p,n,light,dir,dist), pow(smoothstep(0.0,-0.02,dir.y),0.2));
	vec4 col = texture(texColor, TexCoords);
	
	// blending depending on water depth
	float weight = waterDist < 5000.0 ? 1.0/(waterDist) : 1.0;
	float distDiff = clamp(0.2*log(((1.0-weight)*(-geoWorldPos.y/(mapHeight*0.5))+weight*(distScene - waterDist)/mapHeight))+1.0,0.0,1.0);
	
	oceanCol = oceanCol*distDiff + col.xyz*(1.0-distDiff);
		
	// fake SSR
	if (distDiff > 0.0) {
		// interpolate plane normal with ocean normal
		float u = dot(camDir,vec3(0.0,1.0,0.0));
		float quadinterp = pow(u-0.5,2)*2.0+0.5;
		vec3 normal = (quadinterp)*vec3(0.0,1.0,0.0) + (1.0-quadinterp)*n;

		//normal = vec3(0.0,1.0,0.0); // enable to view actual reflection
		
		// angle of reflection
		float ref = dot(normal,camDir);
		ref *= 1.0+(pow(abs(ref),4))*1.5; // correct view angle
		
		vec2 coord = vec2(TexCoords.x, 1.0-(ref*focus+TexCoords.y));
		
		float distDiffBlend = min(distDiff*3.0,1.0); // stronger reflections in deeper areas
		if (coord.x > 0.0 && coord.x < 1.0 && coord.y > 0.0 && coord.y < 1.0 && coord.y > TexCoords.y) // only sample reflection above water
			oceanCol = min(oceanCol + distDiffBlend*0.3*max(texture(texColor, coord).xyz,0.1),1.0);
	} 
	
	//SSAO
	float ao = 1.0;
	if (!globalAO) {
		if (randAO) {
			ivec2 q = ivec2(TexCoords*vec2(1920,1080));
			int iFrame = int(uTime*144);			
			srand( hash(q.x+hash(q.y+hash(1117*(iFrame)))));
		}
		if (distScene < waterDist && abs(geoWorldPos.x) > 0.001)
			ao = max(SSAO(TexCoords),clamp((-geoWorldPos.y*0.1+1.25),0.0,1.0));
		else
			ao = 1.0;
	} else 
		ao = SSAO(TexCoords);
	ao = pow(ao,2.0)*1.0;
	
	
	FragColor = vec4(oceanCol*ao,0.0);
}