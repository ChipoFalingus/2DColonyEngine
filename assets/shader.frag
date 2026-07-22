#version 330 core

in vec2 TexCoords;
in vec3 vColor;

out vec4 FragColor;

uniform sampler2D text;

void main() {
    float alpha = texture(text, TexCoords).r;
    FragColor = vec4(vColor * alpha, alpha);
}


