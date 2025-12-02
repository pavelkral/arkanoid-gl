#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aRow0;
layout(location = 2) in vec4 aRow1;
layout(location = 3) in vec4 aRow2;
layout(location = 4) in vec4 aRow3;
layout(location = 5) in vec4 aColor;

layout(std140, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
};

out vec3 vPos;
out vec3 vNormal;
out vec4 vColor;

void main() {
    mat4 model = mat4(aRow0, aRow1, aRow2, aRow3);
    vec4 worldPos = model * vec4(aPos, 1.0);
    vPos = worldPos.xyz;
    vNormal = normalize(mat3(model) * aPos); // simple normal
    vColor = aColor;
    gl_Position = projection * view * worldPos;
}
