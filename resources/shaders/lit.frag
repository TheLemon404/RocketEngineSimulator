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

vec4 generic_desaturate(vec3 color, float factor)
{
    vec3 lum = vec3(0.299, 0.587, 0.114);
    vec3 gray = vec3(dot(lum, color));
    return vec4(mix(color, gray, factor), 1.0);
}

void main() {
    vec3 lightColor = color * 1.2f;
    vec3 darkColor = color * 0.8f;
    vec3 fresnelColor = color * 1.0f;
    float dot = clamp(dot(-passNormal, lightDirection), ambientLightStrength, 1);
    vec3 finalColor = mix(mix(darkColor, lightColor, dot), fresnelColor, fresnel(1.0));
    outColor = vec4(finalColor, 1.0);
}