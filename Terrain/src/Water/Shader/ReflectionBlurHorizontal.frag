#version 430 core 

layout (location = 0) out vec4 gColor;

uniform sampler2D baseColorTexture;
uniform sampler2D reflectColorTexture;

in vec2 UV;

uniform int blurStrength;

// weights generated with: https://observablehq.com/@jobleonard/gaussian-kernel-calculater

int weightLength = 5;

float weights[5] = float[5](0.0613595978134402,
							0.24477019552960988,
							0.38774041331389975,
							0.24477019552960988,
							0.0613595978134402);

/*
float weights[11] = float[11](	0.000003381766950162007,
								0.0002292725895775324,
								0.005977006954929783,
								0.0605975531721828,
								0.24173031550285376,
								0.38292494002701216,
								0.24173031550285376,
								0.0605975531721828,
								0.005977006954929783,
								0.0002292725895775324,
								0.000003381766950162007);
*/
/*
float weights[19] = float[19](	0.000009677377316317854,
								0.00007774906664655715,
								0.0004886429196522016,
								0.002402737087917116,
								0.009244634006751011,
								0.027834737777226178,
								0.06559086081218723,
								0.12097768865668214,
								0.17466680058027237,
								0.1974129434306977,
								0.17466680058027237,
								0.12097768865668214,
								0.06559086081218723,
								0.027834737777226178,
								0.009244634006751011,
								0.002402737087917116,
								0.0004886429196522016,
								0.00007774906664655715,
								0.000009677377316317854);
*/

void main(){
	
	vec4 bColor = texture(baseColorTexture, UV);
	if (bColor.xyz == vec3(0)) discard;
	vec2 tSize = textureSize(reflectColorTexture, 0);



	gColor = vec4(0);
	int i = 0;
	for (int x = -(weightLength - 1) / 2; x <= (weightLength - 1) / 2; x++) {
		vec2 offsetUV = clamp(UV + vec2(x, 0) / tSize * blurStrength, 0, 1);
		if (texture(baseColorTexture, offsetUV).xyz == vec3(0)) offsetUV = UV;
		gColor += texture(reflectColorTexture, offsetUV) * weights[i];
		i += 1;
	}
}
