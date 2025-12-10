#version 330 core

in vec3 Normal;
in float height;

out vec4 FragColor;

void main()
{
    // Altura normalizada
    float h = clamp(height / 8.0, 0.0, 1.0);

    // Materiales
    vec3 grass = vec3(0.0, 0.6, 0.0);
    vec3 dirt  = vec3(0.55, 0.27, 0.07);
    vec3 rock  = vec3(0.4, 0.4, 0.4);
    vec3 snow  = vec3(1.0, 1.0, 1.0);

    // *** PENDIENTE ***
    vec3 N = normalize(Normal);

    // Si la normal apunta hacia arriba → terreno plano
    // Si apunta hacia el lado → terreno escarpado
    float slope = 1.0 - N.y;  
    slope = clamp(slope, 0.0, 1.0);

    // Mezcla progresiva:
    // primero mezcla grass -> dirt según altura
    vec3 color = mix(grass, dirt, smoothstep(0.2, 0.5, h));

    // donde hay pendiente, mezcla roca
    color = mix(color, rock, slope);

    // Añadir nieve solo en alturas altas
    color = mix(color, snow, smoothstep(0.75, 0.95, h));

    FragColor = vec4(color, 1.0);
}
