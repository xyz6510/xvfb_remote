#version 300 es

precision mediump float;

uniform sampler2D tex_font;
uniform float alpha_cut;

in vec2 uv;
in vec4 fg;
in vec4 bg;

out vec4 frag;

void main(void) 
{
	vec2 tc=uv/vec2(textureSize(tex_font,0));
	vec4 color=texture(tex_font,tc);

	if (color.a>alpha_cut) color=fg;
	else {
			color=bg;
	}

	frag=vec4(color);
}
