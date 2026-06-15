#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragWorldPos;

void main() {
    vec4 worldPos = ubo.model * vec4(inPos, 1.0);
    fragWorldPos = worldPos.xyz;
    gl_Position = ubo.proj * ubo.view * worldPos;
    fragColor = inColor;
}