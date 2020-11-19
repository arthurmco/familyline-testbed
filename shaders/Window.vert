#version 150

in vec3 position;

out vec2 uv;

void main() {
    vec4 pos4 = vec4(position, 1.0);
    uv = (position.xy / 2.0) + 0.5;
    gl_Position = pos4;
}
