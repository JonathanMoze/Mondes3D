#version 330 core

in vec3 var_color;
in vec3 n;
in vec3 l;
in vec3 v;

out vec4 out_color;

uniform int wireframe;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  float diffuse = max(dot(n, l), 0);
  vec3 halfVect = normalize(l+v);
  float spec = pow((max(dot(n, halfVect), 0)), s);
  vec3 outVect = (diffuse*dCol)+(spec * sCol);
  return outVect;
}


void main(void) {
    if(wireframe==1)
        out_color = vec4(1,1,1,1);
    else
        out_color = vec4(blinn(n, v, l, var_color, vec3(1, 1, 1), 16), 1.);
}


//main for solar system
/*void main(void) {
    if(wireframe==1)
        out_color = vec4(1,1,1,1);
    else
        out_color = vec4(var_color,1);
}*/
