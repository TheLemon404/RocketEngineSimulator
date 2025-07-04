#version 450

layout(location = 0) in vec3 inPosition;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transform * vec4(inPosition, 1.0f);
}