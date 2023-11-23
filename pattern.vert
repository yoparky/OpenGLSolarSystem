#version 330 compatibility

uniform float	uTime;		// "Time", from Animate( )
uniform bool    uPlain;
out vec2  	vST;		// texture coords

out vec3 vN; // normal vector
out vec3 vL; // vector from point to light
out vec3 vE; // vector from point to eye
vec3 LIGHTPOSITION = vec3( 5., 5., 0. );

const float PI = 	3.14159265;
const float AMP = 	0.2;		// amplitude
const float W = 	2.;		// frequency

void
main( )
{ 
	vec3 vert = gl_Vertex.xyz;
	vST = gl_MultiTexCoord0.st;
	vert.x = vert.x * abs(sin(uTime * PI * 2) * cos(vert.x/10) + 1);
	vert.y = vert.y * abs(cos(uTime * PI * 4) * cos(vert.y/5) + 1);
	vert.z = vert.z * abs(cos(uTime * PI * 3) * cos(vert.z/7) + 1);
	vec4 ECposition = gl_ModelViewMatrix * vec4( vert, 1. ); // eye coordinate position
	vN = normalize( gl_NormalMatrix * gl_Normal ); // normal vector
	vL = LIGHTPOSITION - ECposition.xyz; // vector from the point to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz; // vector from the point to the eye position

	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );

}
