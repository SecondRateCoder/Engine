#version 410 core

in vec4 my_position;
in float largest_dot;

in vec4 target_position;
in float largest_target_dot;

void main(){
    float temp = dot(my_position, target_position);
    if(temp < larget_dot + larget_target_dot){
        gl_Out = {1, 1, 1, 1}; // In frameBuffer tell that there is collision between meshes.
    }else{gl_Out = {1, 1, 1, 1};}
}
