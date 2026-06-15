#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 pdx = dFdx(fragWorldPos);
    vec3 pdy = dFdy(fragWorldPos);
    
    vec3 normal = normalize(cross(pdx, pdy));
    
    vec3 lightDir = normalize(vec3(1.0) - fragWorldPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0) * fragColor;
    
    outColor = vec4(diffuse, 1.0);
}