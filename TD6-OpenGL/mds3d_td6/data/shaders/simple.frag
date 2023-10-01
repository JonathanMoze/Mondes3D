#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 v_texture;

uniform vec3 lightDir;
/*uniform sampler2D earthTexture;
uniform sampler2D cloudsTexture;
uniform sampler2D earthNightTexture;
uniform sampler2D checkerBoardTexture;*/
uniform sampler2D mipmap_color_texture;

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

/*
  vec4 cloudsCol = texture(cloudsTexture, v_texture);
  vec4 earthCol = texture(earthTexture, v_texture);
  vec4 nightCol = texture(earthNightTexture, v_texture);
  float cosTheta = max(0,dot(normalize(v_normal),lightDir));*/

  /*
  if(cloudsCol.x > 0 ){
    out_color = cosTheta * mix(earthCol, cloudsCol, cloudsCol.x);
  } else {
    out_color = cosTheta * mix(earthCol, cloudsCol, 0);
  }

  out_color = mix(nightCol, out_color, cosTheta);*/

  out_color = texture(mipmap_color_texture, v_texture);
  

  /*out_color = vec4(ambient * v_color + blinn(normalize(v_normal),normalize(v_view), lightDir, v_color, spec_color, shininess),1.0);*/
}