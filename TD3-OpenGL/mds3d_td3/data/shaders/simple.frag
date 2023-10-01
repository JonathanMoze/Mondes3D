#version 330 core


in vec3 var_color;

uniform int mode;

out vec4 out_color;

void main(void) {
    if(mode == 0){
        out_color = vec4(var_color,1);
    } else if(mode == 1){
        out_color = vec4(1);
    }
}
