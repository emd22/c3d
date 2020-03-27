#include <f3d/engine/camera.h>
#include <f3d/engine/window.h>
#include <f3d/engine/log.h>
#include <f3d/engine/math.h>
#include <f3d/engine/types.h>
#include <f3d/engine/shader.h>
#include <f3d/engine/time.h>

#include <math.h>

#include <GL/glew.h>
#include <GL/gl.h>

#define RADIAN_MAX 6.2831

camera_t *selected_camera = NULL;

camera_t camera_new(void) {
    camera_t camera;
    memset(&camera, 0, sizeof(camera_t));
    camera.direction = (vector3f_t){0, 0, 0};
    camera.fov = 75;
    return camera;
}

void camera_clamp_rotation(camera_t *camera) {
    const float y_max = math_deg_to_rad(80);
    const float y_min = -y_max;
    
    // 2*PI
    const float x_limit = 6.2831f;
    
    // Rotation X
    if (camera->rotation.x > x_limit)
        camera->rotation.x = 0;
    else if (camera->rotation.x < 0)
        camera->rotation.x = x_limit;
    // Rotation Y
    if (camera->rotation.y > y_max)
        camera->rotation.y = y_max;
    else if (camera->rotation.y < y_min)
        camera->rotation.y = y_min;
}

void camera_move(camera_t *camera, int direction) {
    switch (direction) {
        case CAMERA_FORWARD:
            camera->direction.y = 0;
            camera->direction = vec3f_mul_v(camera->direction, delta_time);
            camera->direction = vec3f_mul_v(camera->direction, camera->move_speed);
            vec3f_add(&(camera->position), camera->position, (camera->direction));
            break;
        case CAMERA_BACKWARD:
            camera->direction.y = 0;
            camera->direction = vec3f_mul_v(camera->direction, delta_time);
            camera->direction = vec3f_mul_v(camera->direction, camera->move_speed);
            vec3f_sub(&(camera->position), camera->position, (camera->direction));
            break;
        case CAMERA_RIGHT:
            camera->right = vec3f_mul_v(camera->right, delta_time);
            camera->right = vec3f_mul_v(camera->right, camera->move_speed);
            vec3f_add(&(camera->position), camera->position, (camera->right));
            break;
        case CAMERA_LEFT:
            camera->right = vec3f_mul_v(camera->right, delta_time);
            camera->right = vec3f_mul_v(camera->right, camera->move_speed);
            vec3f_sub(&(camera->position), camera->position, (camera->right));
            break;
        default:
            return;
    }    
}

void camera_select(camera_t *camera) {
    math_perspective(
        &camera->mat_projection, 
        math_deg_to_rad(camera->fov), 
        (float)default_window->width/(float)default_window->height,
        0.1f,
        100.0f
    );
    selected_camera = camera;
}
  
void camera_update(camera_t *camera, unsigned shaderid) {
    (void)shaderid;
    camera_clamp_rotation(camera);

    camera->direction = (vector3f_t){
        sin(camera->rotation.x),
        sin(camera->rotation.y),
        cos(camera->rotation.x)
    };
    camera->right = (vector3f_t){
        sin(camera->rotation.x-3.14f/2.0f),
        0,
        cos(camera->rotation.x-3.14f/2.0f)
    };
    camera->up = math_cross(camera->right, camera->direction);
    
    vector3f_t lookto;
    lookto.x = camera->position.x+camera->direction.x;
    lookto.y = camera->position.y+camera->direction.y;
    lookto.z = camera->position.z+camera->direction.z;
    camera->mat_view = math_lookat(camera->position, lookto, camera->up);
}
