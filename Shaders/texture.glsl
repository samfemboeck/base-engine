#type vertex
#version 330 core

layout(location = 0) in vec3 bPosition;
layout(location = 1) in vec4 bColor;
layout(location = 2) in vec2 bTexCoord;
layout(location = 3) in float bTexIndex;

uniform mat4 uView;
uniform mat4 uProjection;

out vec2 vTexCoord;
out vec2 vWorldPos;
out vec4 vColor;
out float vTexIndex;

void main()
{
    vTexCoord = bTexCoord;
    vColor = bColor;
    vTexIndex = bTexIndex;
    gl_Position = uProjection * uView * vec4(bPosition, 1.0);
    vWorldPos = vec2(bPosition); 
}

#type fragment
#version 330 core

layout(location = 0) out vec4 Color;

in vec2 vTexCoord;
in vec4 vColor;
in float vTexIndex;
in vec2 vWorldPos;

uniform sampler2D uTextures[32];
uniform vec2 uLightPositions[5];
uniform int uNumLights;
uniform vec3 uLightAttenuation;

void main()
{
    vec2 lightPos = uLightPositions[0];
    float dist = length(vWorldPos - lightPos);
	float attenuation = 1.0 / (uLightAttenuation.x + uLightAttenuation.y * dist + uLightAttenuation.z * dist * dist);	
    vec4 col = texture(uTextures[int(vTexIndex)], vTexCoord) * vColor;
    if (col.a == 0)
        Color = col;
    else
        Color = mix(vec4(1, 1, 1, 1), col, 1/*attenuation*/);
}