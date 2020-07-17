#version 300 es

precision mediump float;

uniform float brightness;
uniform float contrast;
uniform float saturation;
uniform float gamma;
uniform sampler2D tex_screen;

in vec2 uv;
in float linear_f;

out vec4 frag;

void main()
{
	ivec2 st=ivec2(uv);
	vec4 color;
	if (linear_f>0.0) {
		color=texture(tex_screen,uv);
	} else {
		color=texelFetch(tex_screen,st,0);
	}

	//saturation like in gimp -1.0,1.0
	float mn=min(min(color.r,color.g),color.b);
	float mx=max(max(color.r,color.g),color.b);
	vec3 light=vec3((mn+mx)/2.0);
	vec3 color_sat=mix(color.rgb,light,-saturation);

	//contrast -1.0,1.0
	vec3 color_contrast=((color_sat-0.5)*(1.0+contrast))+0.5;

	//brightness -1.0,1.0
	vec3 color_light=color_contrast+brightness;

	//gamma 0.0,10.0
	vec3 color_gamma=pow(color_light,vec3(1.0/gamma));

	frag=vec4(color_gamma,color.a);
}
