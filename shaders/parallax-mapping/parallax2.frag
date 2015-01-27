//
// Relief mapping with binary search fragment shader
//

varying	vec3 et;
varying	vec3 lt;
varying	vec3 ht;

uniform sampler2D decalMap;
uniform sampler2D heightMap;
uniform	float scale;
uniform int steps;

void main (void)
{
    const float	specExp = 80.0;

    float step = 1.0 / steps;
    vec2 dtex = et.xy * scale / ( steps * et.z ); // adjustment for one layer
    float height = 1.0; // height of the layer
    vec2 tex = gl_TexCoord[0].xy; // our initial guess
    float h = texture2D ( heightMap, tex ).a; // get height

    while ( h < height )
    {
        height -= step;
        tex += dtex;
        h = texture2D( heightMap, tex ).a;
    }

    // now add some iterations of binary search
    vec2 delta = 0.5 * dtex;
    vec2 mid = tex - delta; // midpoint

    for ( int i = 0; i < 5; i++ )
    {
        delta *= 0.5;

        if ( texture2D( heightMap, mid ).a < height )
        {
            mid += delta;
        }
        else
        {
            mid -= delta;
        }
    }

    tex = mid;

    // now offset texture coordinates with height
    vec3 color = texture2D( decalMap, tex ).rgb;
    vec3 n = normalize ( texture2D( heightMap, tex ).rgb * 2.0 - vec3( 1.0 ) );
    float diff = max ( dot ( n, normalize( lt ) ), 0.0 );
    float spec = pow ( max ( dot ( n, normalize( ht ) ), 0.0 ), specExp );

    gl_FragColor = vec4( color * diff + vec3( 0.7 * spec ), 1.0 );
}
