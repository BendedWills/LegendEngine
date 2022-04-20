#version 450

layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 fragColor;

void main() 
{
    vec3 finalColor = inColor.xyz;
    if (!gl_FrontFacing)
        finalColor /= 2;

    fragColor = vec4(finalColor.xyz, 1.0);
}