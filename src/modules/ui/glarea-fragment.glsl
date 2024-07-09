#version 460

precision mediump float; 

flat in vec3 startPos;
in vec3 vertPos;
in vec3 vColor;

out vec4 fragColor;

uniform vec2  u_resolution;
uniform uint  u_pattern;
uniform float u_factor;

void main()
{
    vec2  dir  = (vertPos.xy-startPos.xy) * u_resolution/2.0;
    float dist = length(dir);

    uint bit = uint(round(dist / u_factor)) & 15U;
    if ((u_pattern & (1U<<bit)) == 0U)
        discard; 
    fragColor = vec4(vColor, 1.0);
}