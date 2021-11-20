#version 430

layout (quads) in;

in vec3 tcPosition[];

out vec3 tePosition;
out vec2 tePatchDistance; // 2 components now!

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main(void)
{
    // interpolate in horizontal direction between vert. 0 and 3
    vec3 p0 = mix(tcPosition[0], tcPosition[3], gl_TessCoord.x);
    // interpolate in horizontal direction between vert. 1 and 2
    vec3 p1 = mix(tcPosition[1], tcPosition[2], gl_TessCoord.x);
    // interpolate in vert direction
    vec3 p = mix(p0, p1, gl_TessCoord.y);
    tePatchDistance = gl_TessCoord.xy;
    tePosition = normalize(p); // project on unit sphere
    gl_Position = Projection * Modelview * vec4(tePosition, 1);
}