#version 330 core

in vec3 vtx_position;
in vec3 vtx_normal;
in vec3 vtx_color;

uniform mat4 M_proj;
uniform mat4 M_obj;
uniform mat4 M_view;

uniform vec3 light_pos;
uniform mat3 normal_mat;

out vec3 var_color;
out vec3 v;
out vec3 l;
out vec3 n;


vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  float diffuse = max(dot(n, l), 0);
  vec3 halfVect = normalize(l+v);
  float spec = pow((max(dot(n, halfVect), 0)), s);
  vec3 outVect = (diffuse*dCol)+(spec * sCol);
  return outVect;
}

void main()
{
  

  v = -((M_view * M_obj * vec4(vtx_position, 1.)).xyz);
  normalize(v);
  l = (light_pos);
  normalize(l);
  n = (normal_mat*vtx_normal);
  normalize(n);

  gl_Position = M_proj * M_view * M_obj * vec4(vtx_position, 1.);
  
  //var_color = blinn(n, v, l, vec3(0.1, 0.1, 0.6), vec3(1, 1, 1), 16);

  var_color = vtx_color; //vec3(0.1, 0.1, 0.6); 

  
}

//Main for solar system
/*void main()
{
  

  gl_Position = M_proj * M_view * M_obj * vec4(vtx_position, 1.);

  var_color = vtx_normal;

  
}*/
