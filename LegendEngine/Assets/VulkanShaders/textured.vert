#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 fragCoord;
layout(location = 1) out vec3 fragPos;

layout(set = 0, binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
} cam;

layout(push_constant) uniform Object
{
    mat4 transform;
} obj;

void main() 
{
    vec4 worldPos = obj.transform * vec4(pos.xyz, 1);

    fragCoord = texcoord;
    fragPos = worldPos.xyz;

    gl_Position = cam.projection * cam.view * worldPos;
}