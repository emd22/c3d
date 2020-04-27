#version 420 core

#define MAX_DIR_LIGHTS 1
#define MAX_POINT_LIGHTS 2

struct Material {
    bool use_normalmap;
    bool use_specularmap;
    bool use_diffuse;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float radius;
};

in vec3 frag_vertex;

in vec3 frag_eye_direction;

in vec2 frag_uv;
in vec3 frag_normal;
in vec3 frag_tangent;
in vec3 frag_bitangent;
in vec4 frag_shadow_coords;

layout(location = 0) out vec4 output_colour;

uniform sampler2D shadow_map;

uniform Material material;
uniform vec3 view_pos;

uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir, float visibility);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir);

void main() {
    vec3 norm = frag_normal;
    vec3 normals_texture;
    vec3 n;
    if (material.use_normalmap) {
        // 255/128 is near 2.0, but fixes some issues with heights when using normalmaps
        normals_texture.xy = 255.0f/128.0f * (vec2(1.0) - texture2D(material.normal, frag_uv).rg) - 1.0;
        normals_texture.z = sqrt(1.0 - dot(normals_texture.xy, normals_texture.xy));
        n = normalize(norm);
        n = normalize((frag_tangent * normals_texture.x) + (frag_bitangent * normals_texture.y) + (n * normals_texture.z));    
    }
    else {
        n = normalize(norm);
    }
    float bias = 0.005;
    //float bias = 0.005*tan(acos(dot(n, normalize(dirLights[0].direction)))); // cosTheta is dot( n,l ), clamped between 0 and 1
    //bias = clamp(bias, 0, 0.01);
    float visibility = 1.0f;
    if (texture(shadow_map, frag_shadow_coords.xy).x < frag_shadow_coords.z-bias) {
        visibility = 0.2;
    }
    
    vec3 result = vec3(0);
    for (int i = 0; i < MAX_DIR_LIGHTS; i++) {
        result += CalcDirectionalLight(dirLights[i], n, frag_eye_direction, visibility);
    }
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        result += clamp(CalcPointLight(pointLights[i], n, frag_eye_direction), 0.0, 1.0);
    }
    output_colour = vec4(result, 1.0f);
}

float blinnPhong(vec3 normal, vec3 frag_vertex, vec3 view_pos, vec3 light_dir, float shininess) {
    vec3 eye_dir = normalize(view_pos-frag_vertex);
    vec3 half_vector = normalize(light_dir + eye_dir);
    vec3 n = normalize(normal);
    
    return pow(max(dot(n, half_vector), 0.0), shininess);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir, float visibility) {
    vec3 diffuse_tex = vec3(1);
    vec3 specular_tex = vec3(1);
    
    if (material.use_diffuse) {
        diffuse_tex = vec3(texture(material.diffuse, frag_uv));
    }
    if (material.use_specularmap) {
        specular_tex = vec3(texture(material.specular, frag_uv));
    }
    diffuse_tex *= light.diffuse;
    specular_tex *= light.specular;
    
    vec3 light_dir = normalize(light.direction);
    // diffuse
    //float diff = max(dot(normal, light_dir), 0.0f);
    float ndotl = max(dot(normalize(normal), light_dir), 0.0);
    // specular
    float specularity = blinnPhong(normal, frag_vertex, view_pos, light_dir, material.shininess);
    // final
    vec3 ambient = light.ambient * light.ambient * diffuse_tex;
    vec3 diffuse = vec3(ndotl) * diffuse_tex;
    vec3 specular = vec3(ndotl) * specular_tex * specularity;
    
    //specular *= diff;
    
    return (ambient + diffuse*visibility + specular*visibility);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir) {
    view_dir = normalize(view_dir);
    float radius = light.radius;
    
    vec3 diffuse_tex = vec3(1);
    vec3 specular_tex = vec3(1);
    
    if (material.use_diffuse) {
        diffuse_tex = vec3(texture(material.diffuse, frag_uv));
    }
    if (material.use_specularmap) {
        specular_tex = vec3(texture(material.specular, frag_uv));
    }
    
    diffuse_tex *= light.diffuse;
    specular_tex *= light.specular;
    
    vec3 light_pos = light.position - frag_vertex;
    //float dist = distance(light.position, fragVertex);
    float dist = length(light_pos);
    //dist = dist*dist;
    light_pos = normalize(light_pos);
    vec3 light_dir = normalize(light.position - frag_vertex);

    float specularity = blinnPhong(normal, frag_vertex, view_pos, light_dir, material.shininess);
    
    float ndotl = max(dot(normalize(normal), light_dir), 0.0);
    float attenuation = (1.0 - (dist/radius));

    vec3 mat_ambient  = light.ambient * diffuse_tex;
    vec3 mat_diffuse  = vec3(ndotl)  * diffuse_tex;
    vec3 mat_specular = light.specular * specular_tex * specularity;
    
    mat_ambient  *= attenuation;
    mat_diffuse  *= attenuation;
    mat_specular *= attenuation;
    
    return (mat_ambient + mat_diffuse + mat_specular);
}

