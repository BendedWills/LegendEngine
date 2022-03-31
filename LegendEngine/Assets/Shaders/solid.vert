#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform ObjectBuffer
{
    mat4 transform;
};

void main() 
{
    outColor = color;

    gl_Position = transform * vec4(pos.xy, 0, 1);
}