44000000000000

#define vs
// Compare 2 gameobj's against each other as well as every other one.

layout(location = 1) vec3 pos0;
layout(location = 2) vec3 ldot0;

layout(location = 1) vec3 pos1;
layout(location = 2) vec3 ldot1;

layout(location = 1) vec3 pos2;
layout(location = 2) vec3 ldot2;

layout(location = 2) unsigned int counter;
uniform unsigned int width;

// a: pos0 and pos1 collide, r: pos0 and target collide, g: pos1 and target collide, b: IDK
out bool success1, success2, success3;
void main(){
    success1 = dot(pos0, pos1) < ldot0 + ldot1;
    success2 = dot(pos0, target_pos) < ldot0 + target_ldot;
    success3 = dot(pos1, target_pos) < ldot1 + target_ldot;
    gl_Position = vec4(counter/3, (counter/3) % width, 1, 1);
}
#define shaderend

#define fs

in bool success1, success2, success3;

void main(){
    FragColor = vec4(success1, success2, success3, 0);
}
#define shaderend
