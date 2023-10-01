#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec3 v_view;
in vec2 v_texture;
in mat3 v_normalMat;

uniform vec3 lightDir;
uniform sampler2D normals_map;
uniform sampler2D diffuse_texture;


out vec4 out_color;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  vec3 res = vec3(0,0,0);
  float dc = max(0,dot(n,l));
  if(dc>0) {
    res = dCol * dc;
    float sc = max(0,dot(n,normalize(v+l)));
    if(sc>0)
      res += sCol * pow(sc,s) * dc;
  }
  return res;
}

void main(void) {
  float ambient = 0.0;
  float shininess = 50;
  vec3 spec_color = vec3(1,1,1);

  /*vec3 textureNormal = v_normalMat * ((texture(normals_map, v_texture).xyz*2)-1);
  vec3 textureDiffuse = texture(diffuse_texture, v_texture).xyz;*/
  //out_color = vec4(ambient * textureDiffuse + blinn(normalize(textureNormal),normalize(v_view), lightDir, textureDiffuse, spec_color, shininess),1.0);

  
  normalize(v_tangent);
  normalize(v_bitangent);
  normalize(v_normal);

  mat3 tbnvMatrix = transpose(mat3(v_tangent, v_bitangent, v_normal));

  vec3 lt = normalize(tbnvMatrix * lightDir);
  vec3 vt = normalize(tbnvMatrix * v_view);



  vec3 normalsTexture = v_normalMat * ((texture(normals_map, v_texture).xyz*2)-1);
  vec3 textureDiffuse = texture(diffuse_texture, v_texture).xyz;

  out_color = vec4(ambient * textureDiffuse + blinn(normalize(normalsTexture),normalize(vt), lt, textureDiffuse, spec_color, shininess),1.0);
}