44000000000000

#define vs
// Compare 2 gameobj's against each other as well as every other one.

layout(location = 1) vec3 target_pos;
layout(location = 2) vec3 target_ldot;

uniform unsigned int counter;
uniform unsigned int width;

uniform vec3 pos0;
uniform float ldot0;

uniform vec3 pos1;
uniform float ldot1;

// a: pos0 and pos1 collide, r: pos0 and target collide, g: pos1 and target collide, b: IDK
out bool success1, success2, success3;
void main(){
    success1 = dot(pos0, pos1) < ldot0 + ldot1;
    success2 = dot(pos0, target_pos) < ldot0 + target_ldot;
    success3 = dot(pos1, target_pos) < ldot1 + target_ldot;
    gl_Position = vec4(counter, counter % width, 1, 1);
}
#define shaderend

#define fs

in bool success1, success2, success3;

void main(){
    FragColor = vec4(success1, success2, success3, 0);
}
#define shaderend
