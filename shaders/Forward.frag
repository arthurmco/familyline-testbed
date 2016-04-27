#version 120

uniform vec3 color;
varying vec3 norm_out;

void main() {
  gl_FragColor = vec4(color, 1.0);
}
