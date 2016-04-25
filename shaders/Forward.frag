#version 120

uniform vec3 color;

void main() {
  gl_FragColor = vec4(color.rgb, 1.0);
}
