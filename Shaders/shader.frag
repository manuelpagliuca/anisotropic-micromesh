#version 460 core
out vec4 FragColor;
in vec3 color;
in vec3 pos;

uniform bool wireframe;

vec3 flatNormal() {
    vec3 dx = dFdx(pos);
    vec3 dy = dFdy(pos);
    return normalize(cross(dx, dy));
}

void main() {
    float diffuse = abs(flatNormal().x);
    diffuse = mix(diffuse, 1.0f, 0.3f);

    if (!wireframe)
        FragColor = vec4(color * diffuse, 1.0f);
    else
        FragColor = vec4(0.0f, 0.0f, 0.0f, 0.2f);
}

