#version 330 core

in vec3 v_normal;
in vec3 v_view;
in vec2 v_uv;
in vec4 color;

uniform vec3 lightDir;
uniform sampler2D colormap;

out vec4 out_color;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  vec3 res = vec3(0,0,0);
  float dc = max(0,dot(n,l));
  if(dc>0) {
    res = dCol * dc;
  }
  return res;
}

void main(void) {
  float ambient = 0.4;
  float shininess = 50;
  vec3 spec_color = vec3(1,1,1);

  vec3 texColor = texture(colormap, v_uv).xyz;

  vec3 blinnColor = blinn(normalize(v_normal),normalize(v_view), lightDir, texColor, spec_color, shininess);

  out_color = vec4(ambient * texColor + blinnColor,1.0);
  //out_color = color;
}
