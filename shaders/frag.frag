#version 450

#define MAX_STEPS 100

layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outColor;

float scene(vec3 p) {
    return length(p) - 1.0;
}

vec3 raymarch(vec3 o, vec3 d) {
    vec3 p = o;
    for (int i = 0; i < MAX_STEPS; i++) {
        float dist = scene(p);
        p+=d*dist;
        if (dist < 0.001) {
            return vec3(1.0, 0.0, 0.0) * dot(normalize(p), vec3(0.0, 0.0, 0.0));
        }
    }
    return vec3(0.0, 0.0, 0.0);
}

void main() {
    vec3 cam = vec3(0.0, 0.0, -5.0);
    vec3 ray = normalize(vec3(gl_FragCoord.xy / vec2(800.0, 800.0) * 2.0 - 1.0, 1.0));
    outColor = vec4(raymarch(cam, ray), 1.0);
}