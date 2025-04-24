#version 450

layout(set = 0, binding = 0) uniform UBO {
    mat4 projection;
} ubo;

// Quad vertex input (binding = 0)
layout(location = 0) in vec2 inQuadPos;
layout(location = 1) in vec2 inUV;

// Circle instance input (binding = 1)
layout(location = 2) in vec2 inCirclePos;
layout(location = 3) in float inCircleRadius;
layout(location = 4) in vec3 inCircleColor;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out float fragRadius;

void main() {
    // Expand the quad by radius and translate to circle position
    vec2 scaled = inQuadPos * inCircleRadius;
    vec2 worldPos = inCirclePos + scaled;

    gl_Position = ubo.projection * vec4(worldPos, 0.0, 1.0);

    fragUV = inUV;
    fragColor = inCircleColor;
    fragRadius = inCircleRadius;
}
