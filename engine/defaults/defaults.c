#include "../engine/defaults/defaults.h"

void INPUTH_CAMHandle_0(void *scene_, GLenum key, GLenum press){
    scene_t *scene = scene_;
    volatile double mousePos[2] = {0}, last_mousePos[2] = {0};
    glfwGetCursorPos(scene->parent, &mousePos[0], &mousePos[1]);
    scene->cameras[*scene->loaded_cams].rot[0] += 360/(scene->cameras[*scene->loaded_cams].sensitivity * (mousePos[0] - last_mousePos[0]));
    scene->cameras[*scene->loaded_cams].rot[1] += 360/(scene->cameras[*scene->loaded_cams].sensitivity * (mousePos[1] - last_mousePos[1]));
    switch(key){
        case GLFW_KEY_PAGE_UP:
            vec3 temp_pg_up = {scene->cameras[*scene->loaded_cams].up[0], scene->cameras[*scene->loaded_cams].up[1], scene->cameras[*scene->loaded_cams].up[2]};
            temp_pg_up[2]+=scene->cameras[*scene->loaded_cams].speed;
            scene->cameras[*scene->loaded_cams].pos[0] = temp_pg_up[0]; scene->cameras[*scene->loaded_cams].pos[1] = temp_pg_up[1]; scene->cameras[*scene->loaded_cams].pos[1] = temp_pg_up[1];
            break;
        case GLFW_KEY_PAGE_DOWN:
            vec3 temp_pg_dwn = {scene->cameras[*scene->loaded_cams].up[0], scene->cameras[*scene->loaded_cams].up[1], scene->cameras[*scene->loaded_cams].up[2]};
            temp_pg_dwn[2]-=scene->cameras[*scene->loaded_cams].speed;
            scene->cameras[*scene->loaded_cams].pos[0] = temp_pg_dwn[0]; scene->cameras[*scene->loaded_cams].pos[1] = temp_pg_dwn[1]; scene->cameras[*scene->loaded_cams].pos[1] = temp_pg_dwn[1];
            break;
        case GLFW_KEY_UP:
            vec3 temp_up = {scene->cameras[*scene->loaded_cams].up[0], scene->cameras[*scene->loaded_cams].up[1], scene->cameras[*scene->loaded_cams].up[2]};
            temp_up[1]+=scene->cameras[*scene->loaded_cams].speed;
            scene->cameras[*scene->loaded_cams].pos[0] = temp_up[0]; scene->cameras[*scene->loaded_cams].pos[1] = temp_up[1]; scene->cameras[*scene->loaded_cams].pos[1] = temp_up[1];
            break;
        case GLFW_KEY_DOWN:
            vec3 temp_dwn = {scene->cameras[*scene->loaded_cams].up[0], scene->cameras[*scene->loaded_cams].up[1], scene->cameras[*scene->loaded_cams].up[2]};
            temp_dwn[1]-=scene->cameras[*scene->loaded_cams].speed;
            scene->cameras[*scene->loaded_cams].pos[0] = temp_dwn[0]; scene->cameras[*scene->loaded_cams].pos[1] = temp_dwn[1]; scene->cameras[*scene->loaded_cams].pos[1] = temp_dwn[1];
            break;
        case GLFW_KEY_LEFT:
            vec3 temp_lft = {scene->cameras[*scene->loaded_cams].up[0], scene->cameras[*scene->loaded_cams].up[1], scene->cameras[*scene->loaded_cams].up[2]};
            temp_lft[0]-=scene->cameras[*scene->loaded_cams].speed;
            scene->cameras[*scene->loaded_cams].pos[0] = temp_lft[0]; scene->cameras[*scene->loaded_cams].pos[1] = temp_lft[1]; scene->cameras[*scene->loaded_cams].pos[1] = temp_lft[1];
            break;
        case GLFW_KEY_RIGHT:
            vec3 temp_rgt = {scene->cameras[*scene->loaded_cams].up[0], scene->cameras[*scene->loaded_cams].up[1], scene->cameras[*scene->loaded_cams].up[2]};
            temp_rgt[0]+=scene->cameras[*scene->loaded_cams].speed;
            scene->cameras[*scene->loaded_cams].pos[0] = temp_rgt[0]; scene->cameras[*scene->loaded_cams].pos[1] = temp_rgt[1]; scene->cameras[*scene->loaded_cams].pos[1] = temp_rgt[1];
            break;
    }
    last_mousePos[0] = mousePos[0]; last_mousePos[1] = mousePos[1];
    return;
}