#version 150

in vec3 outColor;

void main() {
    gl_FragColor = vec4(outColor,1);
}