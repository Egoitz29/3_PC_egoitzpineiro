#version 330 core

in vec3 FragPos;
in vec3 Normal;
in float height;

out vec4 FragColor;

uniform vec3 lightDir;   // dirección de la luz
uniform vec3 viewPos;    // posición de la cámara

void main()
{
    // =========================
    // 1. COLOR POR ALTURA (TU SISTEMA)
    // =========================
    float h = clamp(height / 8.0, 0.0, 1.0);

    vec3 grass = vec3(0.0, 0.7, 0.0);
    vec3 dirt  = vec3(0.55, 0.27, 0.07);
    vec3 snow  = vec3(1.0, 1.0, 1.0);

    vec3 baseColor;
    if (h < 0.4)
        baseColor = grass;
    else if (h < 0.75)
        baseColor = dirt;
    else
        baseColor = snow;

    // =========================
    // 2. ILUMINACIÓN PHONG REAL
    // =========================
    vec3 N = normalize(Normal);
    vec3 L = normalize(-lightDir);

    // DIFUSA
    float diff = max(dot(N, L), 0.0);

    // ESPECULAR
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), 32.0);

    vec3 ambient = 0.3 * baseColor;

    vec3 diffuse  = diff * baseColor;
   vec3 specular = vec3(0.2) * spec;


    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0);
}
