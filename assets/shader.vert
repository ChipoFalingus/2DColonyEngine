#version 330 core
layout(location = 0) in vec2 aPos;   // screen position in pixels
layout(location = 1) in vec2 aUV;    // atlas UV
layout(location = 2) in vec3 aColor; // glyph color

out vec2 TexCoords;
out vec3 vColor;

uniform mat4 projection;

void main()
{
    TexCoords = aUV;
    vColor = aColor;
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}