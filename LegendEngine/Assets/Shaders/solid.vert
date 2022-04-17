#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform ObjectBuffer
{
    mat4 transform;
};

layout(set = 1, binding = 0) uniform CameraBuffer
{
    mat4 view;
    mat4 projection;
};

void main() 
{
    outColor = color;

    gl_Position = projection * view * transform * vec4(pos.xyz, 1);
}