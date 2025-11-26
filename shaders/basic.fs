//basic.fs

#version 330 core

in vec3 vertexColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex1;          // piedra
uniform sampler2D tex2;          // tronco
uniform bool useTexture;         // usar textura o color
uniform bool useDoubleTexture;   // usar 1 o 2 texturas
uniform float mixFactor;         // 0.0 = solo tex1, 1.0 = solo tex2

void main()
{
    // Sin textura: solo color de vértice
    if (!useTexture) {
        FragColor = vec4(vertexColor, 1.0);
        return;
    }

    // Una sola textura (cuadrado grande)
    if (!useDoubleTexture) {
        FragColor = texture(tex1, texCoord);
        return;
    }

    // Dos texturas mezcladas (cuadrado pequeño)
    vec4 col1 = texture(tex1, texCoord);
    vec4 col2 = texture(tex2, texCoord);

    // Mezcla de las dos
    FragColor = mix(col1, col2, mixFactor);
}
