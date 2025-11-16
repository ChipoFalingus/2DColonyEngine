#version 330 core
#define MAX_LIGHTS 32

in vec2 TexCoords;
in vec3 vColor;
out vec4 FragColor;

uniform sampler2D text;
uniform bool isUI;

struct Light {
    vec2 position;
    vec3 color;
    float radius;
    float additionalIntensity;
};

uniform int numLights;
uniform Light lights[MAX_LIGHTS];

void main()
{
    float alpha = texture(text, TexCoords).r;

    // discard nearly transparent fragments to avoid fringe
    //if (alpha < 0.53) discard;

    if (isUI) {
        // UI should use per-glyph color as well
        FragColor = vec4(vColor, alpha);
        return;
    }

    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; i++) {
        float dist = distance(gl_FragCoord.xy, lights[i].position);

        // falloff controlled ONLY by radius
        float falloff = 1.0 - clamp(dist / lights[i].radius, 0.0, 1.0);

        // intensity boosted but radius unaffected
        float intensity = falloff * lights[i].additionalIntensity;

        // apply light color modulated by glyph color
        result += vColor * lights[i].color * intensity;
    }

    result *= alpha; // modulate by glyph alpha
    FragColor = vec4(result, alpha);
}