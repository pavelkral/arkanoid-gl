#version 450 core
in vec3 vPos;
in vec3 vNormal;
in vec4 vColor;
out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 lightPos = vec3(10.0, 20.0, 10.0);
    vec3 L = normalize(lightPos - vPos);
    vec3 V = normalize(vec3(0.0, 15.0, 35.0) - vPos); // camera baked
    // simple Blinn-Phong
    float diff = max(dot(N, L), 0.0);
    vec3 H = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 64.0);
    vec3 base = vColor.rgb * 0.6 + vColor.rgb * 0.4 * diff;
    vec3 color = base + vec3(1.0) * 0.6 * spec; // add highlight
    FragColor = vec4(color, vColor.a);
}
