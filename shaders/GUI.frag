#version 150

uniform sampler2D texPanel;
in vec2 uv;
out vec4 color;

uniform float opacity;

void main() {
    color = texture(texPanel, uv);
    color.a *= opacity;
}

