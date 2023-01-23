#version 450 core
out vec4 FragColor;
in vec3 color;
uniform bool wireframe;

void main() {
    if (!wireframe)
        FragColor = vec4(color, 1.0f);
    else
        FragColor = vec4(vec3(0.0f), 1.0f);

}
