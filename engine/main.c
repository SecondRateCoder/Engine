#include <Public.h>
#include <.\engine\Graphics\DrawingProtocol.h>
#include <string.h>

int main(){
    cwd_init();
    win_t *mainw = win_init("MainWindow", 800, 200);
    win_flood(mainw, (argb_t){.9, .7, .03, 1});
    const float sqrt3 = sqrtf(3);
    char *dest = malloc(sizeof(char) * strlen(cwd) + 1);
    strncat(dest, "Resources/Shaders/Shaders.txt", 36);
    shaders_pull(dest);
    free(dest);
    SHADERBLOCK_HANDLE(mainw->shaders, true);
    dest = malloc(sizeof(char)* (strlen(cwd)+1));
    strcat(dest, "Resources/Textures/AnotherBar.jpeg");
    winimage_append(mainw, 800, 800, 4, dest, (argb_t){0, 0, 0, 1});
    GLuint tex0_uni = glGetUniformLocation(mainw->shaders->shaderProgram, "tex0");
    glUniform1i(tex0_uni, 0);
    win_draw(mainw, 
    (GLfloat[]){
        -0.5f, (-0.5f* sqrt3)/3, 0, 08.f, 0.3f, 0.2f,
        0.5f, (-0.5f* sqrt3)/3, 0, 08.f, 0.3f, 0.2f,
        -0.5f, (-0.5f* sqrt3* 2)/3, 0, 08.f, 0.3f, 0.2f,
        -0.5f/2, (0.5f* sqrt3)/6, 0, 08.f, 0.3f, 0.2f,
        0.5f/2, (0.5f* sqrt3)/6, 0, 08.f, 0.3f, 0.2f,
        0/2, (0.5f* sqrt3)/3, 0, 08.f, 0.3f, 0.2f,}, 36, 
    (GLuint[9]){
        0, 3, 5,
        3, 2, 4,
        5, 4, 1}, 9);
    glBindTextureEXT(GL_TEXTURE_2D, mainw->textures[mainw->textures_curr].img);
    win_poll(mainw);
    win_kill(mainw);
    return 1;
}