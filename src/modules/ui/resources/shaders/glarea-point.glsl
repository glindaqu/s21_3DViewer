#version 460

precision mediump float; 

in vec3 vColor;

out vec4 fragColor;

void main()
{    
    float distance = length(gl_PointCoord - vec2(0.5));

    if (distance > 0.5) {
        discard;
    }
    
    fragColor = vec4(vColor, 1.0);
}