#include <Window.h>

int main(){
    win_t *main = win_init("MainWindow", 800, 200);
    win_flood(main, (argb_t){.9, .7, .03, 1});
    win_kill(main);
}