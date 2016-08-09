#version 150

in vec3 position;
in vec3 color;

uniform mat4 mvp;

out vec3 outColor;

void main() {
    vec4 pos4 = vec4(position,1);
    pos4 = mvp * pos4;

    outColor = color;

    gl_Position = pos4;
}
