#version 120

attribute vec3 position;
attribute vec3 normal;

uniform mat4 mvp;
uniform mat4 mModel, mView;

varying vec3 norm_out;

varying vec3 norm_Camera;
varying vec3 light_Camera;

void main() {
  gl_Position = mvp * vec4(position, 1.0);

  vec3 pos_World = (mModel * vec4(position, 1.0)).xyz;
  vec3 pos_Camera = (mView * mModel * vec4(position, 1.0)).xyz;
  vec3 eyeDir_Camera = vec3(0,0,0) - pos_Camera;

  vec3 light_World = vec3(8,2,12);
  vec3 lightPos_Camera = (mView * vec4(light_World, 1.0)).xyz;
  vec3 lightDir_Camera = lightPos_Camera + eyeDir_Camera;

  vec3 normal_Camera = (mView * mModel * vec4(normal, 0.0)).xyz;

  norm_Camera = normal_Camera;
  light_Camera = lightDir_Camera;
}
