#version 330 core

in vec3 v_normal;
in vec2 v_texCoord;

uniform vec3 u_color;

out vec4 FragColor;

void main() {
    // Simple directional lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(normalize(v_normal), lightDir), 0.3);
    
    FragColor = vec4(u_color * diff, 1.0);
}
