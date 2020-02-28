#ifndef C3D_TYPE_VEC_H
#define C3D_TYPE_VEC_H

#include <math.h>

enum {
    // int
    TYPE_VEC2,
    TYPE_VEC3,
    TYPE_VEC4,
    // float
    TYPE_VEC2F,
    TYPE_VEC3F,
    TYPE_VEC4F,
};

#define VEC_FLOAT_DEFINE_SIMPLE_FUNCTIONS_VEC(n, type) \
    inline float vec##n##f_mul_inner(type a, type b) { \
        float p = 0.0f; \
        int i; \
        for (i = 0; i < n; i++) \
            p += vecf_get_at(&b, TYPE_VEC2F+n, i)*vecf_get_at(&a, TYPE_VEC2F+n, i); \
        return p; \
    } \
    inline void vec##n##f_scale(type *r, type v, float s) { \
        int i; \
        for (i = 0; i < n; i++) \
            vecf_set_at(r, TYPE_VEC2F+n, i, vecf_get_at(&v, TYPE_VEC2F+n, i)*s); \
    } \
    inline float vec##n##f_len(type v) { \
        return sqrtf(vec##n##f_mul_inner(v, v)); \
    } \
    inline void vec##n##f_norm(type *r, type v) { \
        const float k = 1.0f/vec##n##f_len(v); \
        vec##n##f_scale(r, v, k); \
    }


// float
typedef struct {
    float x, y, z, w;
} vector4f_t;

typedef struct {
    float x, y, z;
} vector3f_t;

typedef struct {
    float x, y;
} vector2f_t;

// int type
typedef struct {
    int x, y, z, w;
} vector4_t;

typedef struct {
    int x, y, z;
} vector3_t;

float vecf_get_at(void *vector, int type, int index);
int vecf_set_at(void *vector, int type, int index, float value);
vector3f_t vec3f_sub_vec3f(vector3f_t vec0, vector3f_t vec1);

// vec2f_*
//VEC_FLOAT_DEFINE_SIMPLE_FUNCTIONS_VEC(2, vector2f_t)
// vec3f_*
VEC_FLOAT_DEFINE_SIMPLE_FUNCTIONS_VEC(3, vector3f_t)
// vec4f_*
VEC_FLOAT_DEFINE_SIMPLE_FUNCTIONS_VEC(4, vector4f_t)

#endif
