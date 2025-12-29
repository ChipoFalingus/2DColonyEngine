#version 330 core
#define MAX_LIGHTS 32

in vec2 TexCoords;
in vec3 vColor;
out vec4 FragColor;

uniform sampler2D wallMap;
uniform int mapWidth;
uniform int mapHeight;

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


float getWall(ivec2 tile) {
    // convert tile coords to UV
    vec2 uv = gl_FragCoord.xy / vec2(mapWidth, mapHeight);
    return texture(wallMap, uv).r;   // 0.0 or 1.0

}

bool isBlocked(vec2 lightPos, vec2 fragPos) {

    vec2 dir = fragPos - lightPos;
    float distanceTotal = length(dir);
    vec2 stepDir = normalize(dir);

    float stepSize = 0.5; // Tune this for accuracy
    float dist = 0.0;

    while (dist < distanceTotal) {
        vec2 pos = lightPos + stepDir * dist;

        ivec2 tile = ivec2(floor(pos));

        if (getWall(tile) > 0.5)
            return true;

        dist += stepSize;
    }

    return false;
}


void main()
{
    float alpha = texture(text, TexCoords).r;

    if (isUI) {
        FragColor = vec4(vColor, alpha);
        return;
    }

    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; i++) {

     // check if fragment is blocked from this light
    //if (isBlocked(lights[i].position, gl_FragCoord.xy)) continue;

        float dist = distance(gl_FragCoord.xy, lights[i].position);
        float falloff = 1.0 - clamp(dist / lights[i].radius, 0.0, 1.0);
        float intensity = falloff * lights[i].additionalIntensity;

        result += vColor * lights[i].color * intensity;
    }


    result *= alpha;
    FragColor = vec4(result, alpha);
}



