//basic.vs

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec3 vertexColor;
out vec2 texCoord;

uniform float rotation;

void main()
{
    mat2 rot = mat2(
        cos(rotation), -sin(rotation),
        sin(rotation),  cos(rotation)
    );

    vec2 pos = aPos.xy;
    vec2 center = vec2(0.35, -0.35);

    pos -= center;
    pos = rot * pos;
    pos += center;

    gl_Position = vec4(pos, aPos.z, 1.0);

    vertexColor = aColor;
    texCoord = aTex;
}
