#version 450

layout(location = 0) in vec2 inCoord;
layout(location = 0) out vec4 fragColor;

// Set 1 is the material set
layout(set = 1, binding = 0) uniform Material
{
    vec4 color;
} mat;

void main() 
{
    fragColor = vec4(mat.color.xyz, 1.0);
}