/*
	Hyllian's jinc windowed-jinc 2-lobe 3D sharpest with anti-ringing Shader

	Copyright (C) 2011-2015 Hyllian - sergiogdb@gmail.com

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

*/

#version 300 es

precision mediump float;

const   float halfpi            = 1.5707963267948966192313216916398;
const   float pi                = 3.1415926535897932384626433832795;
const   float JINC2_WINDOW_SINC = 0.34;//0.34;
const   float JINC2_SINC        = 0.85;//0.99;//0.85;
const   float wa                = JINC2_WINDOW_SINC*pi;
const   float wb                = JINC2_SINC*pi;
const   float JINC2_AR_STRENGTH = 0.8;//0.8;

const vec3 dtt = vec3(65536,255,1);

float reduce(vec3 A)
{
	return dot(A, dtt);
}


// Calculates the distance between two points
float d(vec2 pt1, vec2 pt2)
{
	vec2 v = pt2 - pt1;
	return sqrt(dot(v,v));
}

vec3 min4(vec3 a, vec3 b, vec3 c, vec3 d)
{
	return min(a, min(b, min(c, d)));
}

vec3 max4(vec3 a, vec3 b, vec3 c, vec3 d)
{
    return max(a, max(b, max(c, d)));
}

vec4 resampler(vec4 x)
{
	vec4 res;

	res = (x==vec4(0.0, 0.0, 0.0, 0.0)) ?  vec4(wa*wb)  :  sin(x*wa)*sin(x*wb)/(x*x);

	return res;
}

uniform sampler2D tex_input;
uniform vec2 input_size;

in vec2 uv;

out vec4 frag;

void main()
{
	vec2 OGL2Size=input_size;
	vec2 texCoord=uv;

	vec3 color, E;
	vec4 weights[4];

	vec2 Dx = vec2(1.0, 0.0);
	vec2 Dy = vec2(0.0, 1.0);
	vec2 OGL2InvSize = 1.0/OGL2Size;

	vec2 dx = vec2(OGL2InvSize.x,0.0);
	vec2 dy = vec2(0.0, OGL2InvSize.y);

	vec2 pc  = texCoord.xy*OGL2Size.xy;
	vec2 tex = (floor(texCoord.xy*OGL2Size) + vec2(0.5, 0.5))/OGL2Size;

	vec2 tc = (floor(pc-vec2(0.5,0.5))+vec2(0.5,0.5));

	weights[0] = resampler(vec4(d(pc, tc    -Dx    -Dy), d(pc, tc           -Dy), d(pc, tc    +Dx    -Dy), d(pc, tc+2.0*Dx    -Dy)));
	weights[1] = resampler(vec4(d(pc, tc    -Dx       ), d(pc, tc              ), d(pc, tc    +Dx       ), d(pc, tc+2.0*Dx       )));
	weights[2] = resampler(vec4(d(pc, tc    -Dx    +Dy), d(pc, tc           +Dy), d(pc, tc    +Dx    +Dy), d(pc, tc+2.0*Dx    +Dy)));
	weights[3] = resampler(vec4(d(pc, tc    -Dx+2.0*Dy), d(pc, tc       +2.0*Dy), d(pc, tc    +Dx+2.0*Dy), d(pc, tc+2.0*Dx+2.0*Dy)));

	tc = tc*OGL2InvSize.xy;

	vec3 c00 = texture(tex_input, tc    -dx    -dy).xyz;
	vec3 c10 = texture(tex_input, tc           -dy).xyz;
	vec3 c20 = texture(tex_input, tc    +dx    -dy).xyz;
	vec3 c30 = texture(tex_input, tc+2.0*dx    -dy).xyz;
	vec3 c01 = texture(tex_input, tc    -dx       ).xyz;
	vec3 c11 = texture(tex_input, tc              ).xyz;
	vec3 c21 = texture(tex_input, tc    +dx       ).xyz;
	vec3 c31 = texture(tex_input, tc+2.0*dx       ).xyz;
	vec3 c02 = texture(tex_input, tc    -dx    +dy).xyz;
	vec3 c12 = texture(tex_input, tc           +dy).xyz;
	vec3 c22 = texture(tex_input, tc    +dx    +dy).xyz;
	vec3 c32 = texture(tex_input, tc+2.0*dx    +dy).xyz;
	vec3 c03 = texture(tex_input, tc    -dx+2.0*dy).xyz;
	vec3 c13 = texture(tex_input, tc       +2.0*dy).xyz;
	vec3 c23 = texture(tex_input, tc    +dx+2.0*dy).xyz;
	vec3 c33 = texture(tex_input, tc+2.0*dx+2.0*dy).xyz;

	color = E = texture(tex_input, texCoord.xy).xyz;

	vec3 F0 = texture(tex_input, tex +dx+0.25*dx +0.25*dy).xyz;
	vec3 F1 = texture(tex_input, tex +dx+0.25*dx -0.25*dy).xyz;
	vec3 F2 = texture(tex_input, tex +dx-0.25*dx -0.25*dy).xyz;
	vec3 F3 = texture(tex_input, tex +dx-0.25*dx +0.25*dy).xyz;

	vec3 H0 = texture(tex_input, tex +0.25*dx +0.25*dy+dy).xyz;
	vec3 H1 = texture(tex_input, tex +0.25*dx -0.25*dy+dy).xyz;
	vec3 H2 = texture(tex_input, tex -0.25*dx -0.25*dy+dy).xyz;
	vec3 H3 = texture(tex_input, tex -0.25*dx +0.25*dy+dy).xyz;

	float f0 = reduce(F0);
	float f1 = reduce(F1);
	float f2 = reduce(F2);
	float f3 = reduce(F3);

	float h0 = reduce(H0);
	float h1 = reduce(H1);
	float h2 = reduce(H2);
	float h3 = reduce(H3);

	bool block_3d = f0==f1 && f1==f2 && f2==f3 && h0==h1 && h1==h2 && h2==h3;

	//  Get min/max samples
	vec3 min_sample = min4(c11, c21, c12, c22);
	vec3 max_sample = max4(c11, c21, c12, c22);

	color = vec3(dot(weights[0], vec4(c00.x, c10.x, c20.x, c30.x)), dot(weights[0], vec4(c00.y, c10.y, c20.y, c30.y)), dot(weights[0], vec4(c00.z, c10.z, c20.z, c30.z)));
	color+= vec3(dot(weights[1], vec4(c01.x, c11.x, c21.x, c31.x)), dot(weights[1], vec4(c01.y, c11.y, c21.y, c31.y)), dot(weights[1], vec4(c01.z, c11.z, c21.z, c31.z)));
	color+= vec3(dot(weights[2], vec4(c02.x, c12.x, c22.x, c32.x)), dot(weights[2], vec4(c02.y, c12.y, c22.y, c32.y)), dot(weights[2], vec4(c02.z, c12.z, c22.z, c32.z)));
	color+= vec3(dot(weights[3], vec4(c03.x, c13.x, c23.x, c33.x)), dot(weights[3], vec4(c03.y, c13.y, c23.y, c33.y)), dot(weights[3], vec4(c03.z, c13.z, c23.z, c33.z)));
	color = color/(dot(weights[0], vec4(1,1,1,1)) + dot(weights[1], vec4(1,1,1,1)) + dot(weights[2], vec4(1,1,1,1)) + dot(weights[3], vec4(1,1,1,1)));

	// Anti-ringing
	vec3 aux = color;
	color = clamp(color, min_sample, max_sample);
	color = mix(aux, color, JINC2_AR_STRENGTH);

	color = block_3d ? color : E;

	// final sum and weight normalization
	frag=vec4(color,1);
}
