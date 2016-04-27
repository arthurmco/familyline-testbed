#version 120

attribute vec3 position;
attribute vec3 normal;

uniform mat4 mvp;

varying vec3 norm_out;

void main() {
  gl_Position = mvp * vec4(position, 1.0);
  norm_out = normal;

}
