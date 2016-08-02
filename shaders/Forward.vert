#version 120

attribute vec3 position;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 mvp;
uniform mat4 mModel, mView;

varying vec3 norm_out;
varying vec2 tex_coords;

varying vec3 norm_Camera;

uniform int lightCount;
uniform vec3 lightPositions[4];

varying vec3 lightDirections[4];

void main() {
  gl_Position = mvp * vec4(position, 1.0);

  vec3 pos_World = (mModel * vec4(position, 1.0)).xyz;
  vec3 pos_Camera = (mView * mModel * vec4(position, 1.0)).xyz;
  vec3 eyeDir_Camera = vec3(0,0,0) - pos_Camera;

  for (int i = 0; i < lightCount; i++) {
      vec3 light_World = lightPositions[i];
      vec3 lightPos_Camera = (mView * vec4(light_World, 1.0)).xyz;
      vec3 lightDir_Camera = lightPos_Camera + eyeDir_Camera;
      lightDirections[i] = lightDir_Camera.xyz;
  }

  vec3 normal_Camera = (mView * mModel * vec4(normal, 0.0)).xyz;

  norm_Camera = normal_Camera;
  tex_coords = texcoord;
}
