#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 outColor;

void main() 
{
    outColor = color;
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}