#version 330 core

in float height;
out vec4 FragColor;

void main()
{
    if (height < 5.0)
        FragColor = vec4(0.1, 0.6, 0.1, 1.0);   // verde
    else if (height < 20.0)
        FragColor = vec4(0.5, 0.35, 0.15, 1.0); // marrón
    else
        FragColor = vec4(0.9, 0.9, 0.9, 1.0);   // nieve
}
