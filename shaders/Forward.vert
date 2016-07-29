#version 120

attribute vec3 position;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 mvp;
uniform mat4 mModel, mView;

varying vec3 normal_Camera;
varying vec2 tex_coords;

varying vec3 eyeDir_Camera;

void main() {
  gl_Position = mvp * vec4(position, 1.0);

  vec3 pos_World = (mModel * vec4(position, 1.0)).xyz;
  vec3 pos_Camera = (mView * mModel * vec4(position, 1.0)).xyz;

  normal_Camera = (mView * mModel * vec4(normal, 0.0)).xyz;
  eyeDir_Camera = vec3(0,0,0) - pos_Camera;

  tex_coords = texcoord;
}
