//
//  Parallax Occlusion mapping fragment shader
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
    const float	minSteps = 5.0;
//    const float	bumpScale = 0.08;
    const float	specExp = 80.0;
    const float	eps = 0.05;

    float numSteps = steps + et.z * ( minSteps - steps );
    float step = 1.0 / numSteps;
    vec2 dtex = -step * scale * et.xy / et.z; // adjustment for one layer
    float height = 0.99; // height of the layer
    vec2 tex = gl_TexCoord[0].xy; // our initial guess
    float h = 1.0 - texture2D( heightMap, tex ).a;

    while ( h < height )
    {
        height -= step;
        tex += dtex;
        h = 1.0 - texture2D( heightMap, tex ).a;
    }

    // now find point via linear interpolation
    vec2 prev = tex - dtex; // previous point
    float hPrev = 1.0 - texture2D ( heightMap, prev ).a - (height + step); // < 0
    float hCur = h - height; // > 0

    tex = mix ( tex, prev, hCur / (hCur - hPrev ) );

    vec4 color = texture2D ( decalMap, tex );
    vec3 n = normalize ( texture2D( heightMap, tex ).rgb * 2.0 - vec3( 1.0 ) );
    float diff = dot ( n, normalize( lt ) );
    float spec = pow ( max( dot( n, normalize( ht ) ), 0.0 ), specExp );

    // now compute shadow
    float vis = 1.0; // light visibility
    float sampleWeight = 9.0 / numSteps; // weight of one sample
    vec2 ltex;

    if ( diff > eps ) // no sense checking other case
    {
        height = 1.0 - color.a;
        step = color.a / numSteps;
        ltex = scale * step * lt.xy / lt.z;
        height += step; // move one step off the surface
        tex += ltex;
        sampleWeight *= pow( 1.0 - lt.z, 3.0 ); // to fight nearly perpendicular lighting

        while ( height < 1.0 - eps )
        {
            if ( 1.0 - texture2D( heightMap, tex ).a > height )
            {
                vis -= sampleWeight;
            }

            height += step;
            tex += ltex;
        }
    }
    else // we're back-facing
    {
        diff = 0.0;
    }

    gl_FragColor = vec4( vis * ( color.rgb * diff + vec3( 0.7 * spec ) ), 1.0 );
}
