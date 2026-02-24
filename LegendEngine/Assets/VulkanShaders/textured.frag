#version 450

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec3 inPos;

layout(location = 0) out vec4 fragColor;

layout(set = 2, binding = 0) uniform Material
{
    vec4 color;
} mat;

layout(set = 2, binding = 1) uniform sampler2D albedoSampler;

const vec3 LIGHT_POS = vec3(-2, 1, 1);
const float LIGHT_STRENGTH = 10;
const float AMBIENT_LIGHT = 0.1;

void main()
{
    float intensity = 1 / pow(distance(LIGHT_POS, inPos), 2);
    intensity *= LIGHT_STRENGTH;

    vec3 finalColor = texture(albedoSampler, inCoord.xy).xyz;
    finalColor *= mat.color.xyz;
    finalColor *= max(intensity, AMBIENT_LIGHT);

    fragColor = vec4(finalColor.xyz, 1.0);
}