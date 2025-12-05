33000000000000

#define vs
// Compare 2 gameobj's against each other as well as every other one.

uniform vec3 pos0;
uniform vec3 ldot0;

layout(location = 0) vec3 pos1;
layout(location = 1) unsigned float ldot1;

layout(location = 2) vec3 pos2;
layout(location = 3) vec3 ldot2;

layout(location = 4) vec3 pos3;
layout(location = 5) unsigned float ldot3;

layout(location = 6) vec3 pos4;
layout(location = 7) unsigned float ldot4;

layout(location = 8) unsigned int counter;
uniform unsigned int width; // Store size_t in encoded state.

// a: pos0 and pos1 collide, r: pos0 and pos2 collide, g: pos0 and pos3 collide, b: pos4
out bool success1, success2, success3, success4;
void main(){
    success1 = dot(pos0, pos1) < ldot0 + ldot1;
    success2 = dot(pos0, pos2) < ldot0 + ldot2;
    success3 = dot(pos0, pos3) < ldot0 + ldot3;
    success4 = dot(pos0, pos4) < ldot0 + ldot4;
    gl_Position = vec4((counter > width? counter%width: counter) counter/width, 1, 1);
}
#define shaderend

#define fs

in bool success1, success2, success3, success4;

void main(){
    FragColor = vec4(success1, success2, success3, success4);
}
#define shaderend
