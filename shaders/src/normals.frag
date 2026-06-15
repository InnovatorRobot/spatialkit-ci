#version 330 core

in vec3 v_normal;

out vec4 FragColor;

void main()
{
    // Visualise surface normals as RGB: remap [-1,1] -> [0,1]
    vec3 color = normalize(v_normal) * 0.5 + 0.5;
    FragColor  = vec4(color, 1.0);
}
