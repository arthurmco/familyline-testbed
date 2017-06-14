#version 150

uniform sampler2D texPanel;
in vec2 uv;
out vec4 color;

void main() {
    color = texture(texPanel, uv);
}

