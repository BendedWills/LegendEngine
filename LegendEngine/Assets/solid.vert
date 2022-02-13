#version 450

layout(location = 0) out vec3 outColor;

void main() 
{
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
    outColor = vec3(0);
}