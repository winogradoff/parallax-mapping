//
// Steep parallax mapping fragment shader
//

varying	vec3 et;
varying	vec3 lt;

uniform sampler2D decalMap;
uniform sampler2D heightMap;
uniform	float scale;
uniform int steps;

void main (void)
{
    float step = 1.0 / steps;
    vec2 dtex = et.xy * scale / ( steps * et.z ); // adjustment for one layer
    float height = 1.0; // height of the layer
    vec2 tex = gl_TexCoord[0].xy; // our initial guess
    float h = texture2D( heightMap, tex ).a; // get height

    while ( h < height )
    {
        height -= step;
        tex += dtex;
        h = texture2D( heightMap, tex ).a;
    }

    // now offset texture coordinates with height
    gl_FragColor = vec4( texture2D( decalMap, tex ).rgb, 1.0 );
}
