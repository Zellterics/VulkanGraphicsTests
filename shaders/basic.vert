#version 450

layout(set = 0, binding = 0) uniform UBO {
    mat4 projection;
} ubo;

layout(push_constant) uniform Transform {
    mat4 transform;
} pc;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    vec4 localPos = vec4(inPosition, 1.0, 1.0);
    vec4 worldPos = pc.transform * localPos;

    gl_Position = ubo.projection * vec4(worldPos.xy, 0.0, 1.0);

    fragColor = inColor;
}