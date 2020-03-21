#include <f3d/engine/light.h>
#include <f3d/engine/log.h>
#include <f3d/engine/shader.h>

#include <stdio.h>
#include <string.h>

// NOTE: edit these macros in shader as well
#define MAX_DIR_LIGHTS   8
#define MAX_POINT_LIGHTS 8
#define MAX_SPOTLIGHTS   8

static light_t lights[MAX_LIGHTS];
static int lights_index = 0;

int count_light_types(int type) {
    int i;
    int amt = 0;
    for (i = 0; i < lights_index; i++) {
        if (lights[i].type == type)
            amt++;
    }
    return amt;
}

light_t *light_new(int type) {
    if (lights_index == MAX_LIGHTS) {
        log_msg(LOG_ERROR, "Light limit (%d) reached\n", MAX_LIGHTS);
        return NULL;
    }
    light_t *light = &lights[lights_index++];
    memset(light, 0, sizeof(light_t));
    light->index = count_light_types(type);
    light->type = type;
    return light;
}

// TODO: replace this with something better
void light_init(light_t *light, unsigned shaderid) {
    if (light == NULL)
        return;
        
    char lightstr[32];
    if (light->type == LIGHT_DIRECTIONAL) {
        sprintf(lightstr, "dirLights[%d].direction", light->index);
        shader_set_vec3f(shaderid, lightstr, light->direction);

        sprintf(lightstr, "dirLights[%d].ambient", light->index);
        shader_set_vec3f(shaderid, lightstr, light->ambient);

        sprintf(lightstr, "dirLights[%d].diffuse", light->index);
        shader_set_vec3f(shaderid, lightstr, light->diffuse);

        sprintf(lightstr, "dirLights[%d].specular", light->index);
        shader_set_vec3f(shaderid, lightstr, light->specular);
    }
    else if (light->type == LIGHT_POINT) {
        sprintf(lightstr, "pointLights[%d].position", light->index);
        shader_set_vec3f(shaderid, lightstr, light->position);

        sprintf(lightstr, "pointLights[%d].ambient", light->index);
        shader_set_vec3f(shaderid, lightstr, light->ambient);

        sprintf(lightstr, "pointLights[%d].diffuse", light->index);
        shader_set_vec3f(shaderid, lightstr, light->diffuse);

        sprintf(lightstr, "pointLights[%d].specular", light->index);
        shader_set_vec3f(shaderid, lightstr, light->specular);
        
        sprintf(lightstr, "pointLights[%d].constant", light->index);
        shader_set_float(shaderid, lightstr, light->constant);
        
        sprintf(lightstr, "pointLights[%d].linear", light->index);
        shader_set_float(shaderid, lightstr, light->linear);
        
        sprintf(lightstr, "pointLights[%d].quadratic", light->index);
        shader_set_float(shaderid, lightstr, light->quadratic);
    }
}

void light_update(light_t *light, unsigned shaderid) {
    if (light == NULL)
        return;
        
    char lightstr[32];
    if (light->type == LIGHT_DIRECTIONAL) {
        sprintf(lightstr, "dirLights[%d].direction", light->index);
        shader_set_vec3f(shaderid, lightstr, light->direction);
    }
    else if (light->type == LIGHT_POINT) {
        sprintf(lightstr, "pointLights[%d].position", light->index);
        shader_set_vec3f(shaderid, lightstr, light->position);
    }
}