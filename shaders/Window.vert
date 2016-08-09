#version 150

in vec3 position;

void main() {
    vec4 pos4 = vec4(position,1);
    gl_Position = pos4;
}
