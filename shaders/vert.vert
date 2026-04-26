#version 450

layout(set = 0, binding = 0) uniform GlobalUniform {
    float time;
} ubo;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

mat4 rotationZ(float angle) {
    return mat4(
        cos(angle), -sin(angle), 0, 0,
        sin(angle),  cos(angle), 0, 0,
        0,           0,          1, 0,
        0,           0,          0, 1
    );
}

void main() {
    mat4 model = rotationZ(ubo.time);
    gl_Position = model * vec4(inPos, 1.0);
    fragColor = inColor;
}