#version 150

uniform sampler2D texPanel;
in mediump vec2 uv;
out mediump vec4 color;
uniform lowp float opacity;

void main() {
    color = texture(texPanel, uv);
    color.a *= opacity;
}

