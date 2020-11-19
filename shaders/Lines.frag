#version 150

in mediump vec3 outColor;
out mediump vec4 color;

void main() {
    color = vec4(outColor,1);
}
