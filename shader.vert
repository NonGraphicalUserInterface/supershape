#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec3 fragColour;

float supershape(float alpha, float m) {
    float a = 1;
    float b = 1;
    
    float n1 = 0.2;
    float n2 = 1.7;
    float n3 = 1.7;

    float t1 = pow(abs((1 / a) * cos(m * alpha / 4)), n2);
    float t2 = pow(abs((1 / b) * sin(m * alpha / 4)), n3);

    return pow(t1 + t2, -1 / n1);
}

// Returns (theta, phi)
vec2 angles(vec3 pos, float rho) {
    float theta = atan(pos.y, pos.x);
    // float phi = acos(pos.z / rho);
    float phi = asin(pos.z / rho);

    return vec2(theta, phi);
}

float map(float value, float a1, float b1, float a2, float b2) {
    float range1 = b1 - a1;
    float range2 = b2 - a2;

    return a2 + (value - a1) * range2 / range1;
}

void main() {
    float PI = 3.141592653589793;

    float rho = 2.0;

    float m = map(sin(ubo.time), -1, 1, 0, 7);

    // Cartesian --> spherical
    vec2 angles = angles(inPosition, rho);

    // Spherical --> superspherical
    float r1 = supershape(angles.x, m);
    float r2 = supershape(angles.y, m);

    float x = rho * r1 * cos(angles.x) * r2 * cos(angles.y);
    float y = rho * r1 * sin(angles.x) * r2 * cos(angles.y);
    float z = rho * r2 * sin(angles.y);

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(x, y, z, rho);
    fragColour = vec3(pow(sin(x), 2.0f), pow(sin(y), 2.0f), pow(sin(z), 2.0f));
}
