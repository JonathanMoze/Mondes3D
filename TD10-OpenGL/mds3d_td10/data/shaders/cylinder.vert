#version 330 core

const float M_PI = 3.14159265359;
const int N_SEGMENT = 3;

uniform mat4 obj_mat;
uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat3 normal_mat;
uniform vec3 lengths;
uniform mat4 MJs[N_SEGMENT];

in vec3 vtx_position;
in vec3 vtx_normal;
in vec2 vtx_texcoord;

out vec3 v_normal;
out vec3 v_view;
out vec2 v_uv;
out vec4 color;

vec3 cylinder(vec2 uv, float r, float l)
{
    return vec3(r*cos(uv.x*2*M_PI), r*sin(uv.x*2*M_PI), l*uv.y);
}

void main()
{
    v_uv  = vtx_texcoord;

    float total_length = lengths.x+lengths.y+lengths.z;
    float v = v_uv.y * total_length;

    int seg_idx=0;
    if(v < lengths[0]){
        seg_idx = 0;
    } else if (v < lengths[0] + lengths[1]){
        seg_idx = 1;
    } else {
        seg_idx = 2;
    }

    vec3 pos = cylinder(v_uv, 0.5, total_length);
    vec3 normal = vec3(pos.x,pos.y,0);


    float alpha=0;
    mat4 matFinal;
    if(v >= (lengths.x - 0.2) && v<=(lengths.x + 0.2)){
        alpha = (v - (lengths.x - 0.2))/(0.4);
        matFinal = ((1-alpha) * MJs[0]) + (alpha * MJs[1]);
    } else if(v >= (lengths.x+lengths.y - 0.2) && v <= (lengths.x+lengths.y + 0.2)){
        alpha = (v - ((lengths.x+lengths.y) - 0.2))/(0.4);
        matFinal = ((1-alpha) * MJs[1]) + (alpha * MJs[2]);
    } else {
        matFinal = MJs[seg_idx];
    }

    vec4 p = view_mat *  matFinal * vec4(pos, 1.);

    mat3 normal_transfo = transpose(inverse(mat3(normal_mat)));

    v_normal = normalize(normal_transfo * normal_mat * normal);
    v_view = normalize(-p.xyz);
    color = vec4(alpha, 0, 0, 0);
    gl_Position = proj_mat * p;
}
