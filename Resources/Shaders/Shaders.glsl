4200000000000000000000000

#define vs

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTex;
layout(location = 1) in vec3 aColor;

out vec3 color;
out vec2 tex_coord;
 
uniform mat4 matrix;
uniform float scale;
uniform vec3 offs;


void main(){
    vec3 out_ = offs + (aPos * (1 + scale));
    gl_Position = matrix * vec4(out_, 1.0f);
    color = aColor;
    tex_coord = aTex;
}

#define shaderend


#define fs

out vec4 FragColor;
in vec3 color;
in vec2 tex_coord;

uniform sampler2D tex0;

void main(){
    // FragColor = mix(texture(tex0, tex_coord), vec4(color, 1.0f), 0.7f); // visualize UVs
    FragColor = mix(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(color, 1.0f), 0.5f);
}

#define shaderend
