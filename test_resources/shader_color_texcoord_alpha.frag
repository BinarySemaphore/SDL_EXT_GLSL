varying vec2 v_texCoord;

void main()
{
    vec4 color;

    vec2 delta = vec2(0.5, 0.5) - v_texCoord;
    float dist = dot(delta, delta);

    color.r = v_texCoord.x;
    color.g = v_texCoord.x * v_texCoord.y;
    color.b = v_texCoord.y;
    color.a = 1.0 - (dist * 4.0);
    gl_FragColor = color;
}