#version 450

layout(location = 0) in vec3 passNormal;

uniform vec3 lightDirection;
uniform float ambientLightStrength;

uniform vec3 color;
uniform vec3 viewDirection;

layout(location = 0) out vec4 outColor;

float fresnel(float amount)
{
    return pow(
    1.0 - clamp(dot(normalize(-passNormal), normalize(viewDirection)), 0.0, 1.0),
    amount
    );
}

void main() {
    vec3 darkColor = color / 5.0f;
    float dot = clamp(dot(-passNormal, lightDirection), ambientLightStrength, 1);
    vec3 finalColor = mix(mix(darkColor, color, dot), color, fresnel(1.0));
    outColor = vec4(finalColor, 1.0);
}