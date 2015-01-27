//
//  Parallax Occlusion mapping vertex shader
//

varying	vec3 lt;
varying	vec3 et;
varying	vec3 ht;

uniform	vec4 light;
uniform	vec4 eye;

void main(void)
{
    vec3 p = vec3( gl_ModelViewMatrix * gl_Vertex ); // transformed point to world space

    vec3 l = normalize( vec3( light ) - p ); // vector to light source
    vec3 e = normalize( vec3( eye ) - p ); // vector to the eye
    vec3 h = l + e;
    vec3 n = gl_NormalMatrix * gl_Normal; // transformed n
    vec3 t = gl_NormalMatrix * gl_MultiTexCoord1.xyz; // transformed t
    vec3 b = gl_NormalMatrix * gl_MultiTexCoord2.xyz; // transformed b

    // now remap l, and e into tangent space
    et = vec3( dot( e, t ), dot( e, b ), dot( e, n ) );
    lt = vec3( dot( l, t ), dot( l, b ), dot( l, n ) );
    ht = vec3( dot( h, t ), dot( h, b ), dot( h, n ) );

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
