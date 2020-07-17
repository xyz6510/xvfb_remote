#version 300 es

precision mediump float;

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
uniform float format;
in vec2 uv;

out vec4 frag;

void main(void) 
{
	ivec2 st=ivec2(uv);
	ivec2 st1=ivec2(uv/format);
	float y=texelFetch(tex_y,st,0).r;
	float u=texelFetch(tex_u,st1,0).r;
	float v=texelFetch(tex_v,st1,0).r;

	y=1.1643*(y-0.0625);
	u=u-0.5;
	v=v-0.5;

	float r=y+1.5958*v;
	float g=y-0.39173*u-0.81290*v;
	float b=y+2.017*u;

	frag=vec4(r,g,b,1);
}
