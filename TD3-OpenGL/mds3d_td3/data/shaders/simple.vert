#version 330 core

in vec3 vtx_position;
in vec3 vtx_color;

out vec3 var_color;

uniform float zoom;
uniform vec2 coord;
uniform int mode;

void main()
{
  vec3 p = vec3(-vtx_position.x+coord.x, vtx_position.y+coord.y,-vtx_position.z);
  if(mode == 1){
    p.z-=1e-3;
  }
  gl_Position = vec4(zoom*p, 1.);
  var_color = vec3(0, 0, 0);
}
