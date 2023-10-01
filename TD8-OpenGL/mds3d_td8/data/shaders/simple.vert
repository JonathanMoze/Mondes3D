#version 330 core
#define M_PI 3.1415926535897932384626433832795

uniform mat4 obj_mat;
uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat3 normal_mat;
uniform float torsionVal;

in vec3 vtx_position;
in vec3 vtx_normal;
in vec2 vtx_texcoord;

out vec3 v_normal;
out vec2 v_uv;

mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

vec3 bezier(float u, vec3 B[4], out mat3 F)
{
  vec3 P1_0 = mix(B[0], B[1], u);
  vec3 P1_1 = mix(B[1], B[2], u);
  vec3 P1_2 = mix(B[2], B[3], u);

  vec3 P2_0 = mix(P1_0, P1_1, u);
  vec3 P2_1 = mix(P1_1, P1_2, u);

  vec3 P_u = mix(P2_0, P2_1, u);

  vec3 t = normalize(P2_1 - P2_0);
  vec3 n = normalize(cross(normalize(P1_1 - P1_0), normalize(P1_2 - P1_1)));
  vec3 b = normalize(cross(t, n));

  F = mat3(t, b, n);


  return P_u;

}

vec3 cylinder(float u, float v, vec3 A, vec3 B, float r)
{

  vec3 p = mix(A, B, u);
  float cylY = r*cos(2*M_PI*v);
  float cylZ = r*sin(2*M_PI*v);

  vec3 b = vec3(0, 1, 0);
  vec3 n = cross(normalize(A-B), b);


  


  return p + b*cylY + n*cylZ;
}

vec3 cylBezierYZ(float u, float v, vec3 B[4], float r, out vec3 n)
{
  mat3 tbn;


  vec3 p = bezier(u, B, tbn);
  float cylY = r*cos(2*M_PI*v);
  float cylZ = r*sin(2*M_PI*v);

  mat3 rotation = rotationMatrix(tbn[0], torsionVal*u*(2*M_PI));

  tbn = rotation*tbn;

  float norm1 = abs(cos(2*M_PI*v)) + abs(sin(2*M_PI*v));

  vec3 pos = p + (tbn[1]*cylY)/norm1 + (tbn[2]*cylZ)/norm1;

  n = normalize(pos - p);

  return pos;
}



void main()
{
  /*vec3 A = vec3(0, 0, 0);
  vec3 B = vec3(1, 1, 0);*/
  float r = 0.2;

  //FIGURE 1
  //vec3 B[4] = vec3[4](vec3(-1,0,2), vec3(-0.3,0,4), vec3(0.3,0,2), vec3(1,0,-0.5));  

  //HELICE
  vec3 B[4] = vec3[4](vec3(-0.5,-1,-1), vec3(1.5, 1,-0.3), vec3(-1.5, 1, 0.3), vec3(0.5,-1,1));

  //Helice 2
  //vec3 B[4] = vec3[4](vec3(-1,-0.5,-1), vec3(-1, 1,-0.3), vec3(1, -1, 0.3), vec3(1,0.5,1));
  

  v_uv  = vtx_texcoord;
  vec3 normal;
  vec3 pos = cylBezierYZ(v_uv.x, v_uv.y, B, r, normal);
  v_normal = normalize(normal_mat * normal);
  vec4 p = view_mat * (obj_mat * vec4(pos, 1.));
  gl_Position = proj_mat * p;

  
}
