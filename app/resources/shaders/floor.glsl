//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform vec3 viewPos;

const vec3 albedo = vec3(0.3, 0.3, 0.32); // asfaltno siva

vec3 calc_dir_light(DirLight light, vec3 normal)
{
    vec3 L = normalize(-light.direction);
    float diff = max(dot(normal, L), 0.0);
    return light.ambient * albedo + light.diffuse * diff * albedo;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 L = normalize(light.position - fragPos);
    float diff = max(dot(normal, L), 0.0);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    return (light.ambient * albedo + light.diffuse * diff * albedo) * attenuation;
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 color = calc_dir_light(dirLight, N) + calc_point_light(pointLight, N, FragPos);
    FragColor = vec4(color, 1.0);
}