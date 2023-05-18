#type vertex
#version 330 core

layout(location = 0) in vec3 bPosition;
layout(location = 1) in vec4 bColor;

uniform mat4 uViewProjection;

out vec4 vColor;

void main()
{
    vColor = bColor;
    gl_Position = uViewProjection * vec4(bPosition, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 Color;

in vec4 vColor;

void main()
{
    Color = vColor;
}