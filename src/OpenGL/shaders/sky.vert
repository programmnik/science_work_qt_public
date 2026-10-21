#version 330 core
layout(location = 0) in vec3 aPos;
out vec3 vDir;

uniform mat4 uProjection;
uniform mat4 uView;   // view БЕЗ трансляции

void main()
{
    vDir = aPos;
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}