#version 300 es

layout(location=0) in vec4 vertex;

precision highp isampler2D;

uniform mat4 projection;
uniform vec2 screen;
uniform isampler2D tex_rgba16i;
uniform sampler2D tex_pal;
uniform float font_char_size;

out vec2 uv;
out vec4 fg;
out vec4 bg;

void main()
{
	ivec2 idx=ivec2(vertex.zw);
	vec4 xyc=vec4(texelFetch(tex_rgba16i,idx,0).rgba);

	vec2 pos=xyc.rg+(vertex.xy*font_char_size);
	gl_Position=projection*vec4(pos,0.0,1.0);

	int pal_idx=int(xyc.a);
	int fg_idx=(pal_idx>>8)&0x3f;
	int bg_idx=pal_idx&0x3f;
	fg=texelFetch(tex_pal,ivec2(fg_idx,0),0);
	bg=texelFetch(tex_pal,ivec2(bg_idx,1),0);

	int chr=int(xyc.b);
	int cy=chr/16;
	int cx=chr-(cy*16);
	cx=2+cx*(8+2);
	cy=2+cy*(8+2);
	uv=vec2(cx,cy)+(vertex.xy*8.0);

}
