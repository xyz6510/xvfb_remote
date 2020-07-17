#version 300 es

layout(location=0) in vec2 vertex;

uniform mat4 projection;
uniform vec2 screen;

out vec2 uv;

void main()
{
	vec2 pos=vertex.xy;
	gl_Position=projection*vec4(pos,0.0,1.0);

	uv=vertex.xy;

	uv=uv*screen;
}
