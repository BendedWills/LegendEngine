#version 450

layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform UBO
{
    float test;
    float green;
};

void main() 
{
    fragColor = vec4(inColor.xyz * test * vec3(1, green, 1), 1.0);
}