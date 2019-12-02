#version 400 core

out vec2 uv;

void main(void)
{
	gl_Position = vec4 (glVertex.xy, 0.0, 1.0);
	gl_Position = sign(gl_Position);
	uv = (vec2( gl_Position.x, gl_Position.y ) + vec2(1.0) ) / vec2(2.0);
}