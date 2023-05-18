#version 460 core
layout (location = 0) in vec3 position;

uniform mat4 model;

out vec3 color;
out vec3 pos;

void main() {
    gl_Position = model * vec4(position, 1.0);
    color = vec3(1.0);
    pos = position;
    // color = clamp(position, 0.2, 1.0);
}
