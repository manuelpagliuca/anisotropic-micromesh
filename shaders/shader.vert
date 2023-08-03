#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 meshColor;
out vec3 pos;
out vec3 norm;

void main() {
  gl_Position = projection * view * model * vec4(position, 1.0);
  meshColor = vec3(1.0f, 1.0f, 1.0f);
  pos = position;
  norm = mat3(transpose(inverse(model))) * normal;
}
