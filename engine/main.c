#include <Public.h>

int main(){
    win_t *mainw = win_init("MainWindow", 800, 200);
    win_flood(mainw, (argb_t){.9, .7, .03, 1});
    const float sqrt3 = sqrtf(3);
    win_draw(mainw, 
    (GLfloat[]){
        -0.5f, (-0.5f* sqrt3)/3, 0, 08.f, 0.3f, 0.2f,
        0.5f, (-0.5f* sqrt3)/3, 0, 08.f, 0.3f, 0.2f,
        -0.5f, (-0.5f* sqrt3* 2)/3, 0, 08.f, 0.3f, 0.2f,
        -0.5f/2, (0.5f* sqrt3)/6, 0, 08.f, 0.3f, 0.2f,
        0.5f/2, (0.5f* sqrt3)/6, 0, 08.f, 0.3f, 0.2f,
        0/2, (0.5f* sqrt3)/3, 0, 08.f, 0.3f, 0.2f,},
    36, 
    (GLuint[9]){
        0, 3, 5,
        3, 2, 4,
        5, 4, 1
    }, 9);
    win_poll(mainw);
    win_kill(mainw);
}