#version 320 es
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float lineWidth;
uniform mat4 mvp_matrix;

void main()
{
    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

    vec2 dir = normalize(p2.xy - p1.xy);
    vec2 normal = vec2(-dir.y, dir.x);

    vec4 offset = vec4(normal * lineWidth / 2.0, 0.0, 0.0);

    gl_Position = mvp_matrix * (p1 + offset);
    EmitVertex();

    gl_Position = mvp_matrix * (p1 - offset);
    EmitVertex();

    gl_Position = mvp_matrix * (p2 + offset);
    EmitVertex();

    gl_Position = mvp_matrix * (p2 - offset);
    EmitVertex();

    EndPrimitive();
}