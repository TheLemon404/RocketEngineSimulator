#version 450

uniform vec4 tint;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = tint;
}