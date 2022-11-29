#version 430 core 

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gReflection;

layout(std140) uniform CameraData {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    vec4 Position;
} Camera;

uniform sampler2D dudvTexture;
uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D posTexture;
uniform sampler2D normalTexture;
uniform sampler2D foamTexture;
uniform sampler2D foamBlendTexture;
uniform sampler2D worldPosTexture;

uniform sampler2D skyBackTexture;
uniform sampler2D skyBotTexture;
uniform sampler2D skyFrontTexture;
uniform sampler2D skyLeftTexture;
uniform sampler2D skyRightTexture;
uniform sampler2D skyTopTexture;

uniform vec2 nearFarPlane;
uniform float time;
uniform vec2 windDirection;
uniform bool doSSR;
uniform float ssrRes;

in vec3 POS;
in vec3 CAM;
in vec3 SCREENUV;
in vec2 UV;
in float WIDTH;
in vec3 NORMAL;
in mat3 TANGENTSPACE;

vec4 getSkyboxColor(vec3 dir) {
	vec3 absDir = abs(dir);

	if (absDir.x > absDir.y && absDir.x > absDir.z) {
		if (dir.x > 0) return texture(skyRightTexture, (dir.zy / absDir.x * vec2(-0.5) + vec2(0.5)));
		else return texture(skyLeftTexture, (dir.zy / absDir.x * vec2(0.5, -0.5) + vec2(0.5)));
	}
	else if (absDir.y > absDir.x && absDir.y > absDir.z) {
		if (dir.y > 0) return texture(skyTopTexture, (dir.xz / absDir.y) * vec2(0.5) + vec2(0.5));
		else return texture(skyBotTexture, (dir.xz / absDir.y) * vec2(0.5, -0.5) + vec2(0.5));
	}
	else if (absDir.z > absDir.x && absDir.z > absDir.y) {
		if (dir.z > 0) return texture(skyBackTexture, (dir.xy / absDir.z * vec2(0.5, -0.5) + vec2(0.5)));
		else return texture(skyFrontTexture, (dir.xy / absDir.z * vec2(-0.5) + vec2(0.5)));
	}
	return vec4(0);
}


// https://gist.github.com/983/e170a24ae8eba2cd174f
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
// https://gist.github.com/983/e170a24ae8eba2cd174f
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec4 reflectionColor(vec3 normal, vec3 SCREENUV) {
	float maxDistance = 300;
	int steps = 100;
	float resolution = maxDistance / steps;
	
	vec2 tSize = textureSize(colorTexture, 0);
	vec3 reflectDirection = normalize(reflect(normalize(POS - CAM), normal));
	vec4 end = vec4(POS + reflectDirection * maxDistance, 1);
	vec3 posIncrement = (end.xyz - POS) / steps;

	if (!doSSR) return getSkyboxColor(reflectDirection);
	
	vec2 uv;
	float depth;
	float sampleDepth;
	vec3 middlePos = POS;
	bool hit = false;

	for(int i = 0; i < steps; ++i) {
		middlePos += posIncrement;
		vec4 mid = Camera.ProjectionMatrix * Camera.ViewMatrix * vec4(middlePos, 1);
		uv = mid.xy / mid.w / 2 + 0.5;
		posIncrement *= 1.05;

		if (uv.x > 1 || uv.x < 0 || uv.y > 1 || uv.y < 0 ) {
			return getSkyboxColor(reflectDirection);
		}

		depth = distance(middlePos, CAM);
		
		//float sampleDepth = distance(texture(posTexture, uv).xyz, CAM);
		vec3 wPos = texture(worldPosTexture, uv).xyz;
		sampleDepth = distance(CAM, wPos);
		
		//return vec4(sampleDepth);
		 


		if (sampleDepth < depth && !(wPos == vec3(0))) {
			hit = true;
			break;
		}
	}

	

	if (!hit) {
			return getSkyboxColor(reflectDirection);
	}
	else if (sampleDepth > distance(POS, CAM)) {
		float scaleFactor = 1;// - pow(length((uv - 0.5) * 2), 8);

		float visibility = 1 - max(dot(normalize(CAM - POS), reflectDirection), 0);
		if (scaleFactor > 0) {
			vec4 ret = mix(getSkyboxColor(reflectDirection), texture(colorTexture, uv), visibility);// texture(colorTexture, uv), visibility);
			ret = ret * scaleFactor;
			ret.w = scaleFactor;
			return ret;
		}
	}

	return getSkyboxColor(reflectDirection);
	
}

/*
vec4 reflectionColor(vec3 normal, vec3 SCREENUV) {
	float maxDistance = distance(CAM, POS);
	float resolution = lowQuality ? 8 : 2;
	int steps = 0;
	vec2 tSize = textureSize(colorTexture, 0);

	vec3 reflectDirection = normalize(reflect(normalize(POS - CAM), normal));

	vec4 end = vec4(POS + reflectDirection * maxDistance, 1);
	vec4 endPos = Camera.ProjectionMatrix * Camera.ViewMatrix * end;

	vec2 startUV = SCREENUV.xy;

	vec2 endUV = endPos.xy / endPos.w / 2 + 0.5;
	float startDepth = SCREENUV.z;
	float endDepth = endPos.z / endPos.w / 2 + 0.5;
	startDepth = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (startDepth * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x));
	endDepth = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (endDepth * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x));
	vec2 startFrag = startUV * tSize;
	vec2 endFrag = endUV * tSize;
	vec2 delta = endFrag - startFrag;
	steps = int(length(delta) / resolution);
	
	vec2 increment = (endUV - startUV) / steps;
	float depthIncrement = (endDepth - startDepth) / steps;
	
	vec2 frag = startFrag;
	vec2 uv = startUV;
	float depth = startDepth;
	float sampleDepth;
	

	bool hit = false;
	for(int i = 0; i < steps; ++i) {
		depth += depthIncrement;
		uv += increment;

		if (uv.x > 1 || uv.x < 0 || uv.y > 1 || uv.y < 0 ) return getSkyboxColor(reflectDirection);
		
		sampleDepth = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (texture(depthTexture, uv).r * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x));
		
		if ((sampleDepth < depth) && (sampleDepth > startDepth)) {
			hit = true;
			break;
		}
	}

	if (!hit) return getSkyboxColor(reflectDirection);
	else {
		float scaleFactor = 1;
		float visibility = 1 - max(dot(normalize(CAM - POS), reflectDirection), 0);

		if (scaleFactor > 0) {
			vec4 ret = mix(getSkyboxColor(reflectDirection), texture(colorTexture, uv), visibility);
			ret = ret * scaleFactor;
			ret.w = scaleFactor;
			return ret;
		}
	}

	return getSkyboxColor(reflectDirection);
}
*/

vec3 getFoamColor(float depth) {
	float foamUVScale = 1;
	float maxDepth = 0.5;
	


	
	//if (speedScale < minSpeed) return vec3(0);
	if (depth > maxDepth) return vec3(0);
	
	float borderScale = clamp(5 - abs(UV.y - 0.5) * 16, 0, 1);
	vec3 foamScale = texture(foamBlendTexture, vec2((1 - depth / maxDepth) * borderScale, 0)).rgb;// * clamp((speedScale - minSpeed) / (maxSpeed - minSpeed), 0, 1);


	vec3 temp = texture(foamTexture, UV * vec2(1, WIDTH) * foamUVScale + vec2(time / 10.1, 0)).rgb;
	temp = temp + texture(foamTexture, UV * vec2(1, WIDTH) * foamUVScale + vec2(-time / 10.2, 0)).rgb;
	temp = temp + texture(foamTexture, UV * vec2(1, WIDTH) * foamUVScale + vec2(0, time / 10.3)).rgb;
	temp = temp + texture(foamTexture, UV * vec2(1, WIDTH) * foamUVScale + vec2(0, -time) / 10.4).rgb;
	temp = temp * foamScale / 4;

	return vec3(temp.r + temp.g + temp.b);
}


void main(){
	vec3 screenUV = vec3(gl_FragCoord.xy / textureSize(colorTexture, 0), gl_FragCoord.z);

	float scaledTime = time * 1;
	float phase1 = mod(time, 1);
	float phase2 = mod(time + 0.5, 1);
	float phaseMixValue = abs(phase1 * 2 - 1);

	float uvScale = 3;
	float speedScale = (1 - NORMAL.y) * 7.5 + (1 - 2 * abs(UV.y - 0.5)) * 1.5;
	vec2 uvCord1 = (UV - vec2(phase1 * speedScale, 0)) / uvScale * vec2(1, WIDTH);//vec2(-POS.x, -POS.z) / uvScale + windDirection * time / 30;
	vec2 uvCord2 = (UV - vec2(phase2 * speedScale, 0)) / uvScale * vec2(1, WIDTH);

	vec3 sunPos  = vec3(-1000000000, 600000000, 1000000000);
	vec4 baseBlue = vec4(9, 12, 17, 255) / 255;
	vec4 baseSkyColor = vec4(17*3, 28*3, 50*3, 55) / 255;

	vec2 dudv = (mix(texture(dudvTexture, uvCord1).rg, texture(dudvTexture, uvCord2).rg, phaseMixValue) * 2 - 1) * (distance(POS, CAM) < 300 ? 0.01 : 0);
	vec3 N = normalize(TANGENTSPACE * mix(texture(normalTexture, uvCord1).xyz, texture(normalTexture, uvCord2).xyz, phaseMixValue) * vec3(0.01, 1, 0.01));


	vec3 sunDir = normalize(sunPos - POS);
	vec3 viewDir = normalize(Camera.Position.xyz - POS);
	vec3 halfwayDir = normalize(sunDir + viewDir);

	vec4 reflectColor = reflectionColor(N, screenUV);
	vec4 backgroundColor = vec4(texture(colorTexture, clamp(screenUV.xy + dudv, 0, 1)));
	backgroundColor = vec4(hsv2rgb(rgb2hsv(backgroundColor.rgb) * vec3(1, 0.3, 1)), 1);

	float R = dot(N, viewDir);

	float depthBackground = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (texture(depthTexture, clamp(screenUV.xy, 0.01, 0.99)).r * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
	float depthWater = 2.0 * nearFarPlane.x * nearFarPlane.y / (nearFarPlane.x + nearFarPlane.y - (screenUV.z * 2.0 - 1) * (nearFarPlane.y - nearFarPlane.x)) / (nearFarPlane.y - nearFarPlane.x);
	float depthColorScale = 1 - clamp((depthBackground - depthWater) * nearFarPlane.y * 0.2, 0, 1);

	float shininess = 300;
	float spec = pow(max(dot(normalize(N * vec3(1, 10, 1)), halfwayDir), 0.0), shininess);
	

	// COLOR ---------------------------------------------------------

	gColor = mix(vec4(0) + spec, mix(baseBlue, backgroundColor, depthColorScale), clamp(pow(R, 0.5), 0, 1));

	//gAlbedoSpec += texture(foamTexture, UV * 5) * (((depthBackground - depthWater) * nearFarPlane.y) * speedScale > 0.2 ? 0 : 1);

	gColor = vec4(gColor.rgb + getFoamColor((depthBackground - depthWater) * nearFarPlane.y), 1);
	
	//gAlbedoSpec = vec4(mix(texture(normalTexture, uvCord1).xyz, texture(normalTexture, uvCord2).xyz, phaseMixValue), 1);

	gColor = vec4(gColor.rgb, R);
	//gColor = vec4(0, 0, 1, 1);
	gReflection = reflectColor;
}
