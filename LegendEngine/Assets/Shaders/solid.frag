#version 450

layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform ObjectBuffer
{
    float brightness;
};

void main() 
{
    fragColor = vec4(inColor.xyz * brightness, 1.0);
}