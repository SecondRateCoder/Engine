#include <./engine/Graphics/Window/Window.h>

int main(){
    win_t *mainw = win_init("MainWindow", 800, 200);
    win_flood(mainw, (argb_t){.9, .7, .03, 1});
    const float sqrt3 = sqrtf(3);
    win_draw(mainw, 
    (GLfloat[9]){
        -0.5f, (-0.5f* sqrt3)/3, 0,
        0.5f, (-0.5f* sqrt3)/3, 0,
        -0.5f, (-0.5f* sqrt3* 2)/3, 0,
    }, 9);
    win_kill(mainw);
}