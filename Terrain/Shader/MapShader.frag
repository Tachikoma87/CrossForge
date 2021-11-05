#version 330 core

const float MAP_HEIGHT = 100;
const ivec3 OFFSET = ivec3(-1, 0, 1);

const int LAYER_COUNT = 6;
const vec3 COLORS[LAYER_COUNT] = vec3[](vec3(0, 0, 204) / 255, vec3(252, 208, 70) / 255, vec3(51, 205, 0) / 255,
                                        vec3(32, 129, 0) / 255, vec3(68, 68, 68) / 255, vec3(	255, 250, 250) / 255);
const float LAYER_HEIGHTS[LAYER_COUNT - 1] = float[](0.08, 0.22, 0.37, 0.52, 0.8);
const float BLEND_VALUES[LAYER_COUNT - 1] = float[](0.1, 0.1, 0.1, 0.1, 0.2);

in vec3 FragPosition;
in vec2 SamplePosition;
in float Height;

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

uniform sampler2D HeightMap;

vec3 calculateLayerColor(float height) {
    vec3 color = COLORS[0];

    for (int i = 0; i < LAYER_COUNT - 1; i++) {
        // smoothly interpolate between the different layers
        float drawStrength = smoothstep(-BLEND_VALUES[i] / 2, BLEND_VALUES[i] / 2, height - LAYER_HEIGHTS[i]);

        color = mix(color, COLORS[i + 1], drawStrength);
    }

    return color;
}

float getHeight(vec4 sampledHeight) {
    return sampledHeight.x * MAP_HEIGHT;
}

vec3 calculateNormal(vec2 samplePosition) {
    //     10
    //     |
    // 01--11--21
    //     |
    //     12
    float s01 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.xy));
    float s21 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.zy));
    float s10 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.yx));
    float s12 = getHeight(textureOffset(HeightMap, samplePosition, OFFSET.yz));

    vec3 normal = vec3(s21 - s01, 2, s12 - s10) / 2;

    return normal;
}

void main(){
    vec3 normal = calculateNormal(SamplePosition);
    vec3 color = mix(vec3(1, 0, 0), vec3(0, 0, 1),  Height / MAP_HEIGHT);
    color = vec3(0, 0, Height / MAP_HEIGHT + 0.1);
    color = calculateLayerColor(Height / MAP_HEIGHT);

    gPosition = vec4(FragPosition, 0);
    gNormal = vec4(normal, 0);
    gAlbedoSpec = vec4(color, 0);
}
