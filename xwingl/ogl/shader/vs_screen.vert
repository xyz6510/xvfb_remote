#version 300 es

layout(location=0) in vec2 vertex;

uniform mat4 projection;
uniform vec2 fb_size;
uniform vec2 screen;
uniform vec2 desktop;
uniform float linear;

out vec2 uv;
out float linear_f;

void main()
{
	vec2 pos=vertex.xy;
	gl_Position=projection*vec4(pos,0.0,1.0);

	uv=vertex.xy;
	uv.y=1.0-uv.y;

	linear_f=linear;

	if (linear<1.0) {
		vec2 scr=screen;
		if (scr.x>desktop.x) scr.x=desktop.x;
		if (scr.y>desktop.y) scr.y=desktop.y;
		uv=uv*scr;
	} else {
		uv=uv*vec2(screen.x/fb_size.x,screen.y/fb_size.y);
	}
}
