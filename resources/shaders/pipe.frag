#version 450

layout(location = 0) in vec3 passNormal;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 darkColor;
uniform vec3 fresnelColor;
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
    float dot = clamp(dot(-passNormal, lightDirection), 0, 1);
    vec3 finalColor = mix(mix(darkColor, lightColor, dot), fresnelColor, fresnel(1.0));
    outColor = vec4(finalColor, 1.0);
}