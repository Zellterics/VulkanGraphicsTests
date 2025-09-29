#version 450

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 circleColor;
layout(location = 2) in float fragRadius; //USE FOR EMPTY CIRCLES

layout(location = 0) out vec4 outColor;

void main() {
    float dist = length(uv);
    float alpha = smoothstep(1.0, 0.98, dist);

    if (dist > 1.0)
        discard;

    outColor = vec4(circleColor, 1.0 - alpha);
}
