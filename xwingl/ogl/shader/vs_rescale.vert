#version 300 es

layout(location=0) in vec2 vertex;

uniform mat4 projection;
uniform vec2 fb_size;
uniform vec4 screen;


out vec2 uv;

void main()
{
	vec2 pos=vertex.xy;
	gl_Position=projection*vec4(pos,0.0,1.0);

	float zx=screen.x,zw=screen.z,zy=screen.y,zh=screen.w;
	float px=1.0/fb_size.x;
	float py=1.0/fb_size.y;
	uv=vertex.xy;
	uv=vec2(zx*px,zy*py)+(uv*vec2(zw*px,zh*py));

}
